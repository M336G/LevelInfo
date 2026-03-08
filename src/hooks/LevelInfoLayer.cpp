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
        std::string m_decompressedLevelString;

        async::TaskHolder<utils::web::WebResponse> m_listener;
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
    
    // If the level is updated, avoid displaying the label twice by deleting the first
    // one and letting it create a new updated one
    void levelUpdateFinished(GJGameLevel* level, UpdateResponse response) {
        if (auto label = static_cast<CCLabelBMFont*>(this->getChildByIDRecursive("level-info-label"_spr)))
            label->removeFromParentAndCleanup(true);

        return LevelInfoLayer::levelUpdateFinished(level, response);
    };

	void showLevelInfo(GJGameLevel* level) {
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

        if (SettingsManager::Toggles.objectCount) {
            switch (level->m_objectCount) {
                case 0:
                case 65535: {
                    std::string_view const placeholder = "Objects: Loading...";
                    labelContent << placeholder << std::endl;

                    m_fields->m_decompressedLevelString = ZipUtils::decompressString(
                        level->m_levelString, false, 0
                    );
                    
                    async::spawn(
                        [self]() -> arc::Future<size_t> {
                            co_return co_await async::runtime().spawnBlocking<size_t>([self] {
                                size_t count = 0;

                                for (auto& object : asp::iter::split(self->m_fields->m_decompressedLevelString, ';')) {
                                    if (!object.empty() && !object.starts_with("k"))
                                        count++;
                                }

                                return count;
                            });
                        },
                        [self, placeholder](size_t count) {
                            std::string labelContent = self->m_fields->m_label->getString();
                            auto pos = labelContent.find(placeholder);
                            if (pos != std::string::npos)
                                labelContent.replace(
                                    pos,
                                    placeholder.length(),
                                    fmt::format(
                                        "Objects: ~{}",
                                        Utils::FormatNumber(count)
                                    )
                                );

                            self->m_fields->m_label->setString(labelContent.c_str(), true);
                            self->updateLayout();
                        }
                    );
                    break;
                }
                default:
                    labelContent << "Objects: " << Utils::FormatNumber(level->m_objectCount)
                        << std::endl;
                    break;
            }
        }

        if (SettingsManager::Toggles.ldmObjectCount) {
            if (level->m_lowDetailMode) {
                std::string_view const placeholder = "Objects (LDM): Loading...";
                labelContent << placeholder << std::endl;

                if (m_fields->m_decompressedLevelString.empty()) {
                    m_fields->m_decompressedLevelString = ZipUtils::decompressString(
                        level->m_levelString, false, 0
                    );
                }
                
                async::spawn(
                    [self]() -> arc::Future<size_t> {
                        co_return co_await async::runtime().spawnBlocking<size_t>([self] {
                            size_t count = 0;
                                
                            for (auto& object : asp::iter::split(self->m_fields->m_decompressedLevelString, ';')) {
                                if (object.empty())
                                    continue;

                                if (!object.starts_with("k") && object.find(",103,1,") == std::string::npos && !object.ends_with(",103,1"))
                                    count++;
                            }

                            return count;
                        });
                    },
                    [self, placeholder](size_t count) {
                        std::string labelContent = self->m_fields->m_label->getString();
                        auto pos = labelContent.find(placeholder);
                        if (pos != std::string::npos)
                            labelContent.replace(
                                pos,
                                placeholder.length(),
                                fmt::format(
                                    "Objects (LDM): ~{}",
                                    Utils::FormatNumber(count)
                                )
                            );

                        self->m_fields->m_label->setString(labelContent.c_str(), true);
                        self->updateLayout();
                    }
                );
            } else {
                labelContent << "Objects (LDM): N/A" << std::endl;
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
                auto cached = SentCacheManager::GetLevel(level->m_levelID);

                if (cached) {
                    labelContent << "Sent: " << (cached.value() ? "Yes" : "No")
                        << std::endl;
                } else {
                    std::string const placeholder = "Sent: Loading...";
                    labelContent << placeholder << std::endl;
                    
                    auto req = utils::web::WebRequest();
                    auto levelID = static_cast<int>(level->m_levelID);

                    // This will call SendDB's API to know if the level was sent or
                    // not. The actual sent field will be updated after the request
                    // is completed
                    m_fields->m_listener.spawn(
                        req.get(fmt::format("https://api.senddb.dev/api/v1/level/{}", levelID)),
                        [self, placeholder, levelID](utils::web::WebResponse result) {                            
                            matjson::Value body = result.json().unwrapOrDefault();

                            bool isSent = body.size() > 0 &&
                                body.contains("sends") && body["sends"].size() > 0;
                            if (body.size() > 0)
                                SentCacheManager::SaveLevel(levelID, isSent);

                            std::string labelContent = self->m_fields->m_label->getString();
                            size_t pos = labelContent.find(placeholder);
                            if (pos != std::string::npos)
                                labelContent.replace(
                                    pos,
                                    placeholder.length(),
                                    fmt::format(
                                        "Sent: {}",
                                        body.size() > 0
                                            ? isSent
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