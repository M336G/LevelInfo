#include <Geode/modify/LevelInfoLayer.hpp>
#include <cue/LoadingCircle.hpp> // This may be overkill
#include <asp/iter.hpp>

#include "../managers/SettingsManager.h"
#include "../managers/SentCacheManager.h"
#include "../utils/Utils.h"

using namespace geode::prelude;

class $modify(MyLevelInfoLayer, LevelInfoLayer) {
    struct Fields {
        cue::LoadingCircle* m_loadingCircle = cue::LoadingCircle::create();
        CCLabelBMFont* m_label;

        CCPoint const m_position = SettingsManager::Display.getPosition();
    };

    bool init(GJGameLevel* level, bool challenge) {
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
    void levelDownloadFinished(GJGameLevel* level) {
		LevelInfoLayer::levelDownloadFinished(level);

        if (SettingsManager::Display.size > 0 && SettingsManager::Toggles.anyEnabled())
            showLevelInfo(level);
    };

	void showLevelInfo(GJGameLevel* level) {
        // Remove any duplicate first (caused by level updates, ...)
        if (auto* label = static_cast<CCLabelBMFont*>(this->getChildByIDRecursive("level-info-label"_spr)))
            label->removeFromParentAndCleanup(true);

        Ref<MyLevelInfoLayer> self = this;
        std::stringstream labelContent;

        // Here I define every stats depending on them being enabled or not
        if (SettingsManager::Toggles.requestedStars)
            labelContent << "Requested " << (level->isPlatformer() ? "Moons" : "Stars") << ": " <<
                Utils::FormatNumber(level->m_starsRequested) // Exclusively for trollers on GDPSs
                << std::endl;
        
		if (SettingsManager::Toggles.featuredRank)
            labelContent <<  "Featured Rank: " << (level->m_featured > 0
                    ? Utils::FormatNumber(level->m_featured)
                    : "N/A")
                << std::endl;

        // This may be a little confusing but this was merged to avoid running the
        // decompression twice asynchronously (as it blocks the main thread if it's synchronous)
        if (SettingsManager::Toggles.objectCount || SettingsManager::Toggles.ldmObjectCount) {
            static std::string_view constexpr objectCountLoadingPlaceholder = "Objects: Loading...";
            static std::string_view constexpr ldmObjectCountLoadingPlaceholder = "Objects (LDM): Loading...";

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
                        labelContent << "Objects: " << Utils::FormatNumber(level->m_objectCount)
                            << std::endl;
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
                    labelContent << "Objects (LDM): N/A" << std::endl;
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
                                for (auto& object : asp::iter::split(decompressedLevelString, ';')) {
                                    if (object.empty() || object.starts_with("k"))
                                        continue;

                                    if (objectCountIndicatorNeedsUpdate)
                                        total++;

                                    // 103,1 = high detail object
                                    if (ldmObjectCountIndicatorNeedsUpdate && object.find(",103,1,") == std::string::npos && !object.ends_with(",103,1"))
                                        ldm++;
                                }

                                return { total, ldm };
                            }
                        );
                    },
                    [self, objectCountIndicatorNeedsUpdate, ldmObjectCountIndicatorNeedsUpdate](std::pair<size_t, size_t> count) {
                        if (!self->m_fields->m_label)
                            return;
                        
                        std::string labelContent = self->m_fields->m_label->getString();

                        // If the object count indicator was in the loading state, update
                        // it with our calculated object count
                        if (objectCountIndicatorNeedsUpdate) {
                            auto pos = labelContent.find(objectCountLoadingPlaceholder);
                            if (pos != std::string::npos)
                                labelContent.replace(pos, objectCountLoadingPlaceholder.length(),
                                    fmt::format("Objects: ~{}", Utils::FormatNumber(count.first)));
                        }

                        // If the LDM object count indicator was in the loading state,
                        //update it with our calculated object count in LDM
                        if (ldmObjectCountIndicatorNeedsUpdate) {
                            auto pos = labelContent.find(ldmObjectCountLoadingPlaceholder);
                            if (pos != std::string::npos)
                                labelContent.replace(pos, ldmObjectCountLoadingPlaceholder.length(),
                                    fmt::format("Objects (LDM): ~{}", Utils::FormatNumber(count.second)));
                        }

                        self->m_fields->m_label->setString(labelContent.c_str(), true);
                        self->updateLayout();
                    }
                );
            }
        }

        if (SettingsManager::Toggles.gameVersion)
            labelContent << "Game Version: " << Utils::GetGameVersion(level->m_gameVersion) << std::endl;

        if (SettingsManager::Toggles.twoPlayerMode)
            labelContent << "2-Player Mode: " << (level->m_twoPlayerMode ? "Yes" : "No")
                << std::endl;
        
		if (SettingsManager::Toggles.levelVersion)
            labelContent << "Level Version: " << Utils::FormatNumber(level->m_levelVersion) // I mean, why not
                << std::endl;
        
		if (SettingsManager::Toggles.ldmExistence)
            labelContent << "Has LDM: " << (level->m_lowDetailMode ? "Yes" : "No")
                << std::endl;
        
        if (SettingsManager::Toggles.sent) {
            if (level->m_stars == 0) {
                if (auto cached = SettingsManager::Other.enableSentCache ? SentCacheManager::GetLevel(level->m_levelID) : std::nullopt) {
                    labelContent << "Sent: " << (cached.value() ? "Yes" : "No")
                        << std::endl;
                } else {
                    static std::string_view constexpr placeholder = "Sent: Loading...";
                    labelContent << placeholder << std::endl;

                    auto levelID = static_cast<int>(level->m_levelID);
                    
                    async::spawn(
                        [levelID]() -> arc::Future<Result<bool, void>> {
                            // First try requesting to my own cache API specifically for
                            // the sent state
                            auto req = co_await utils::web::WebRequest()
                                .userAgent(Utils::GetUserAgent())
                                .timeout(std::chrono::seconds(3))
                                .get(fmt::format("https://sdbc.m336.dev/level/{}", levelID));
                            auto body = req.json().unwrapOrDefault();
                            auto error = body["error"].asString().unwrapOrDefault();

                            // If that doesn't work, fallback to the original SendDB API
                            if (!req.ok() || body.size() <= 0 || error.size() > 0) {
                                log::warn(
                                    "Failed requesting to the cache API ({}), fallback to the original SendDB API",
                                    error.size() > 0 ? error : req.errorMessage()
                                );

                                req = co_await utils::web::WebRequest()
                                    .userAgent(Utils::GetUserAgent())
                                    .timeout(std::chrono::seconds(3))
                                    .get(fmt::format("https://api.senddb.dev/api/v1/level/{}", levelID));
                                body = req.json().unwrapOrDefault();
                                error = req.errorMessage();

                                // If that still doesn't work, don't go further
                                if (!req.ok() || body.size() <= 0) {
                                    log::error(
                                        "Failed requesting to the SendDB API: {}",
                                        error.size() > 0 ? error : req.string().unwrap()
                                    );
                                    co_return Err();
                                }

                                // For the SendDB API, just check if the sends object is more than 0
                                co_return Ok(body["sends"].size() > 0 ? true : false);
                            }

                            // For the cache, it's directly indicated in the sent boolean
                            co_return Ok(body["sent"].asBool().unwrap());
                        },
                        [self, levelID](Result<bool, void> result) {
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
                                    fmt::format(
                                        "Sent: {}",
                                        !failed
                                            ? sent
                                                ? "Yes"
                                                : "No"
                                            : "Failed"
                                    )
                                );

                            self->m_fields->m_label->setString(labelContent.c_str(), true);
                            self->updateLayout();
                        }
                    );
                }
            } else {
                SentCacheManager::DeleteLevel(level->m_levelID);
                labelContent << "Sent: Rated" << std::endl;
            }
        }
        
		if (SettingsManager::Toggles.levelID)
            labelContent << "Level ID: " << level->m_levelID << std::endl;

		if (SettingsManager::Toggles.originalLevel)
            labelContent << "Original ID: " <<
                (level->m_originalLevel == 0 || level->m_levelID == level->m_originalLevel
                ? "N/A" : std::to_string(level->m_originalLevel)) << std::endl;

        if (SettingsManager::Toggles.editorTime)
            labelContent << "Editor: " <<
                Utils::FormatTime(std::chrono::seconds(level->m_workingTime)) << std::endl;

        if (SettingsManager::Toggles.editorTimeCopies)
            labelContent << "Edit. (+cop.): " <<
                Utils::FormatTime(std::chrono::seconds(level->m_workingTime + level->m_workingTime2))
                << std::endl;

        if (SettingsManager::Toggles.attempts)
            labelContent << "Attempts: " << Utils::FormatNumber(level->m_attempts)
                << std::endl;
        
		if (SettingsManager::Toggles.jumps)
            labelContent << "Jumps: " << Utils::FormatNumber(level->m_jumps)
                << std::endl;
        
		if (SettingsManager::Toggles.clicks)
            labelContent << "Clicks (best att.): " << Utils::FormatNumber(level->m_clicks)
                << std::endl;
        
		if (SettingsManager::Toggles.attemptTime)
            labelContent << "Time (best att.): " <<
                Utils::FormatTime(std::chrono::seconds(static_cast<int>(level->m_attemptTime)))
                << std::endl;

        // Once we got all the values for the label, create it and set its settings
        m_fields->m_label = CCLabelBMFont::create(
            labelContent.str().c_str(),
            "bigFont.fnt"
        );
        m_fields->m_label->setID("level-info-label"_spr);

        m_fields->m_label->setScale(SettingsManager::Display.size);
        m_fields->m_label->setOpacity(SettingsManager::Display.opacity);
        m_fields->m_label->setColor(SettingsManager::Display.color);

        m_fields->m_label->setPosition(m_fields->m_position);

        // And display it
        this->addChild(m_fields->m_label);
		this->updateLayout();

        // After the label's displayed, finally remove the loading circle
        if (m_fields->m_loadingCircle)
            m_fields->m_loadingCircle->fadeOut();
    };
};