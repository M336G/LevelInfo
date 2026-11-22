#include <Geode/modify/LevelInfoLayer.hpp>
#include <cue/LoadingCircle.hpp> // This may be overkill
#include <asp/iter.hpp>

#include "../managers/SettingsManager.hpp"
#include "../managers/SentCacheManager.hpp"
#include "../utils/Utils.hpp"

using namespace geode::prelude;

class $modify(MyLevelInfoLayer, LevelInfoLayer) {
    struct Fields {
        cue::LoadingCircle *m_loadingCircle = cue::LoadingCircle::create();
        geode::Label *m_label = nullptr;

        CCPoint const m_position = SettingsManager::Display.getPosition();
    };

    bool init(GJGameLevel *level, bool challenge) {
        if (!LevelInfoLayer::init(level, challenge))
            return false;

        if (SettingsManager::Display.size > 0 && SettingsManager::Toggles.anyEnabled()) {
            m_fields->m_loadingCircle->addToLayer(this);

            m_fields->m_loadingCircle->setScale(0.5f);
            m_fields->m_loadingCircle->setPosition(m_fields->m_position);

            m_fields->m_loadingCircle->fadeIn();
            
            // This will be called if the level is already downloaded
            if (!level->m_levelString.empty())
                showLevelInfo(level);
        }

        return true;
    };

    // This will be called if the level just finished downloading
    void levelDownloadFinished(GJGameLevel *level) {
		LevelInfoLayer::levelDownloadFinished(level);

        if (SettingsManager::Display.size > 0 && SettingsManager::Toggles.anyEnabled())
            showLevelInfo(level);
    };

	void showLevelInfo(GJGameLevel *level) {
        // Remove any duplicate first (caused by level updates, ...)
        if (auto *label = static_cast<geode::Label *>(this->getChildByIDRecursive("level-info-label"_spr)))
            label->removeFromParentAndCleanup(true);

        Ref<MyLevelInfoLayer> self = this;
        std::stringstream labelContent;

        // Here I define every stats depending on them being enabled or not
        if (SettingsManager::Toggles.requestedStars)
            labelContent << Utils::FormatIndicator(
                fmt::format("Requested {}", level->isPlatformer() ? "Moons" : "Stars"),
                level->isPlatformer() ? ":moon:" : ":star:",
                Utils::FormatNumber(level->m_starsRequested) // Exclusively for trollers on GDPSs
            ) << std::endl;
        
		if (SettingsManager::Toggles.featuredRank)
            labelContent <<  Utils::FormatIndicator(
                "Featured Rank",
                ":chart:",
                level->m_featured > 0
                    ? Utils::FormatNumber(level->m_featured)
                    : "N/A"
            ) << std::endl;

        // This may be a little confusing but this was merged to avoid running the
        // decompression twice asynchronously (as it blocks the main thread if it's synchronous)
        if (SettingsManager::Toggles.objectCount || SettingsManager::Toggles.ldmObjectCount) {
            std::string const objectCountLoadingPlaceholder = Utils::FormatIndicator("Objects", ":skullbomb:", "Loading...");
            std::string const ldmObjectCountLoadingPlaceholder = Utils::FormatIndicator("Objects (LDM)", ":bomb:", "Loading...");

            bool objectCountIndicatorNeedsUpdate = false;
            bool ldmObjectCountIndicatorNeedsUpdate = false;

            // If the object count indicator was toggled on, return the stored object
            // count if it's not 0 or 65,535 (hardcoded limit on RobTop's servers). If
            // its any of these two, then set it to a loading state and count manually
            // after some other checks. If it wasn't toggled on just don't display it
            if (SettingsManager::Toggles.objectCount) {
                switch (level->m_objectCount) {
                    case 0:
                    case 65535:
                        labelContent << objectCountLoadingPlaceholder << std::endl;
                        objectCountIndicatorNeedsUpdate = true;
                        break;
                    default:
                        labelContent << Utils::FormatIndicator(
                            "Objects",
                            ":skullbomb:",
                            Utils::FormatNumber(level->m_objectCount)
                        ) << std::endl;
                        break;
                }
            }

            // If the LDM object count indicator was toggled on and there is a low
            // detail mode on the level, set it to a loading state and count manually
            // later on. If there's no LDM on the level then just set it to N/A and
            // if it's not toggled on don't display it
            if (SettingsManager::Toggles.ldmObjectCount) {
                if (level->m_lowDetailMode) {
                    labelContent << ldmObjectCountLoadingPlaceholder << std::endl;
                    ldmObjectCountIndicatorNeedsUpdate = true;
                } else {
                        labelContent << Utils::FormatIndicator(
                            "Objects (LDM)",
                            ":bomb:",
                            "N/A"
                        ) << std::endl;
                }
            }

            // This is where it'll count the amount of objects for each indicator (if needed)
            if (objectCountIndicatorNeedsUpdate || ldmObjectCountIndicatorNeedsUpdate) {
                async::spawn(
                    [self, levelString = level->m_levelString, objectCountIndicatorNeedsUpdate, ldmObjectCountIndicatorNeedsUpdate]() -> arc::Future<std::pair<size_t, size_t>> {
                        co_return co_await async::runtime().spawnBlocking<std::pair<size_t, size_t>>(
                            [levelString, objectCountIndicatorNeedsUpdate, ldmObjectCountIndicatorNeedsUpdate]() -> std::pair<size_t, size_t> {
                                auto decompressedLevelString = ZipUtils::decompressString(
                                    levelString, false, 0
                                );

                                size_t total = 0;
                                size_t ldm = 0;

                                // Before I forget what that corresponds to https://boomlings.dev/resources/client/level-components/level-string
                                // {object};{object};{object};...
                                for (auto object : asp::iter::split(decompressedLevelString, ';')) {
                                    if (object.empty() || Utils::ObjectIsLevelSettings(object))
                                        continue;

                                    const bool isObjectBluePortal = Utils::ObjectHasKeyValue(object, "1", "747");

                                    if (Utils::ObjectHasKeyValue(object, "103", "1"))
                                        ldm += isObjectBluePortal ? 2 : 1;

                                    total += isObjectBluePortal ? 2 : 1;
                                }

                                return { total, ldm };
                            }
                        );
                    },
                    [self, objectCountLoadingPlaceholder, ldmObjectCountLoadingPlaceholder, objectCountIndicatorNeedsUpdate, ldmObjectCountIndicatorNeedsUpdate](std::pair<size_t, size_t> count) {
                        if (!self->m_fields->m_label)
                            return;
                        
                        std::string labelContent = self->m_fields->m_label->getString();

                        // If the object count indicator was in the loading state, update
                        // it with our calculated object count
                        if (objectCountIndicatorNeedsUpdate) {
                            auto pos = labelContent.find(objectCountLoadingPlaceholder);
                            if (pos != std::string::npos)
                                labelContent.replace(pos, objectCountLoadingPlaceholder.length(),
                                    Utils::FormatIndicator("Objects", ":skullbomb:", Utils::FormatNumber(count.first)));
                        }

                        // If the LDM object count indicator was in the loading state,
                        // update it with our calculated object count in LDM
                        if (ldmObjectCountIndicatorNeedsUpdate) {
                            auto pos = labelContent.find(ldmObjectCountLoadingPlaceholder);
                            if (pos != std::string::npos) {
                                labelContent.replace(
                                    pos,
                                    ldmObjectCountLoadingPlaceholder.length(),
                                    Utils::FormatIndicator(
                                        "Obj. (LDM)",
                                        ":bomb:",
                                        fmt::format(
                                            "{} (-{}%)",
                                            Utils::FormatNumber(count.first - count.second),
                                            static_cast<int>(std::round(100.f * count.second / count.first))
                                        )
                                    )
                                );
                            }
                        }

                        self->m_fields->m_label->setText(labelContent.c_str());
                        self->updateLayout();
                    }
                );
            }
        }

        if (SettingsManager::Toggles.gameVersion)
            labelContent << Utils::FormatIndicator(
                "Game Version",
                ":gear:",
                Utils::GetGameVersion(level->m_gameVersion)
            ) << std::endl;

        if (SettingsManager::Toggles.twoPlayerMode)
            labelContent << Utils::FormatIndicator(
                "2-Player Mode",
                ":raisedhands:",
                level->m_twoPlayerMode ? "Yes" : "No"
            ) << std::endl;
        
		if (SettingsManager::Toggles.levelVersion)
            labelContent << Utils::FormatIndicator(
                "Level Version",
                ":quest:",
                Utils::FormatNumber(level->m_levelVersion) // I mean, why not
            ) << std::endl;
        
		if (SettingsManager::Toggles.ldmExistence)
            labelContent << Utils::FormatIndicator(
                "Has LDM",
                ":spike:",
                level->m_lowDetailMode ? "Yes" : "No"
            ) << std::endl;
        
        if (SettingsManager::Toggles.sent /*&& (!Utils::IsOnGdps() || !SettingsManager::Other.customSendsEndpoint.empty())*/) {
            if (level->m_stars == 0) {
                if (auto cached = SettingsManager::Other.enableSentCache ? SentCacheManager::GetLevel(level->m_levelID) : std::nullopt) {
                    labelContent << Utils::FormatIndicator(
                        "Sent",
                        ":rate:",
                        cached.value() ? "Yes" : "No"
                    ) << std::endl;
                } else {
                    std::string const placeholder = Utils::FormatIndicator("Sent", ":rate:", "Loading...");
                    labelContent << placeholder << std::endl;

                    auto levelID = static_cast<int>(level->m_levelID);
                    
                    async::spawn(
                        Utils::CheckIfLevelSent(levelID),
                        [self, levelID, placeholder](Result<bool, void> result) {
                            if (!self->m_fields->m_label)
                                return;
                            
                            bool failed = !result.isOk();
                            bool sent = failed ? false : result.unwrap();

                            if (SettingsManager::Other.enableSentCache && !failed)
                                SentCacheManager::SaveLevel(levelID, sent);

                            std::string labelContent = self->m_fields->m_label->getString();
                            size_t pos = labelContent.find(placeholder);
                            if (pos != std::string::npos)
                                labelContent.replace(
                                    pos,
                                    placeholder.length(),
                                    Utils::FormatIndicator(
                                        "Sent",
                                        ":rate:",
                                        !failed
                                            ? sent
                                                ? "Yes"
                                                : "No"
                                            : "Failed"
                                    )
                                );

                            self->m_fields->m_label->setText(labelContent.c_str());
                            self->updateLayout();
                        }
                    );
                }
            } else {
                SentCacheManager::DeleteLevel(level->m_levelID);
                labelContent << Utils::FormatIndicator(
                    "Sent",
                    ":rate:",
                    "Rated"
                ) << std::endl;
            }
        }
        
		if (SettingsManager::Toggles.levelID)
            labelContent << Utils::FormatIndicator(
                "Level ID",
                ":question:",
                utils::numToString(level->m_levelID.value())
            ) << std::endl;

		if (SettingsManager::Toggles.originalLevel)
            labelContent << Utils::FormatIndicator(
                "Original ID",
                ":exclaim:",
                level->m_originalLevel == 0 || level->m_levelID == level->m_originalLevel
                    ? "N/A" : utils::numToString(level->m_originalLevel.value())
            ) << std::endl;

        if (SettingsManager::Toggles.editorTime)
            labelContent << Utils::FormatIndicator(
                "Editor",
                ":tools:",
                Utils::FormatTime(std::chrono::seconds(level->m_workingTime))
            ) << std::endl;

        if (SettingsManager::Toggles.editorTimeCopies)
            labelContent << Utils::FormatIndicator(
                "Edit. (+cop.)",
                ":download:",
                Utils::FormatTime(std::chrono::seconds(level->m_workingTime + level->m_workingTime2))
            ) << std::endl;

        if (SettingsManager::Toggles.attempts)
            labelContent << Utils::FormatIndicator(
                "Attempts",
                ":skull:",
                Utils::FormatNumber(level->m_attempts)
            ) << std::endl;
        
		if (SettingsManager::Toggles.jumps)
            labelContent << Utils::FormatIndicator(
                "Jumps",
                ":chartup:",
                Utils::FormatNumber(level->m_jumps)
            ) << std::endl;
        
		if (SettingsManager::Toggles.clicks)
            labelContent << Utils::FormatIndicator(
                "Clicks (best att.)",
                ":arrowright:",
                Utils::FormatNumber(level->m_clicks)
            ) << std::endl;
        
		if (SettingsManager::Toggles.attemptTime)
            labelContent << Utils::FormatIndicator(
                "Time (best att.)",
                ":clock:",
                Utils::FormatTime(std::chrono::seconds(static_cast<int>(level->m_attemptTime)))
            ) << std::endl;

        // Once we got all the values for the label, create it and set its settings
        m_fields->m_label = geode::Label::create(
            labelContent.str().c_str(),
            "bigFont.fnt"
        );
        m_fields->m_label->setID("level-info-label"_spr);

        m_fields->m_label->setScale(SettingsManager::Display.size);
        m_fields->m_label->setOpacity(SettingsManager::Display.opacity);
        m_fields->m_label->setColor(SettingsManager::Display.color);

        m_fields->m_label->setAnchorPoint({ 0.55f, 0.55f });
        m_fields->m_label->setPosition(m_fields->m_position);

        // And display it
        this->addChild(m_fields->m_label);
		this->updateLayout();

        // After the label's displayed, finally remove the loading circle
        if (m_fields->m_loadingCircle)
            m_fields->m_loadingCircle->fadeOut();
    };
};