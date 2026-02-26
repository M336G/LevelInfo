#include <Geode/modify/LevelInfoLayer.hpp>
#include <cue/LoadingCircle.hpp> // This may be overkill
#include "ModManager.hpp"

class $modify(MyLevelInfoLayer, LevelInfoLayer) {
    struct Fields {
        cue::LoadingCircle *m_loadingCircle = cue::LoadingCircle::create();
        cocos2d::CCLabelBMFont *m_label;

        cocos2d::CCPoint m_position = {
            ModManager::Settings.getWidth(),
            ModManager::Settings.getHeight()
        };

        geode::async::TaskHolder<geode::utils::web::WebResponse> m_listener;
    };

    bool init(GJGameLevel* level, bool challenge) {
        if (!LevelInfoLayer::init(level, challenge))
            return false;

        if (ModManager::Settings.size > 0 && ModManager::Toggles.anyEnabled()) {
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

        if (ModManager::Settings.size > 0 && ModManager::Toggles.anyEnabled())
            showLevelInfo(level);
    };

	void showLevelInfo(GJGameLevel* level) {
        std::stringstream labelContent;

        // Here I define every stats depending on them being enabled or not
        if (ModManager::Toggles.requestedStars)
            labelContent << fmt::format(
                "Requested {}: {}",
                level->isPlatformer() ? "Moons" : "Stars",
                level->m_starsRequested
            ) << std::endl;
        
		if (ModManager::Toggles.featuredRank)
            labelContent << fmt::format(
                "Featured Rank: {}",
                level->m_featured > 0
                    ? std::to_string(level->m_featured)
                    : "N/A"
                ) << std::endl;

        if (ModManager::Toggles.objectCount) {
            switch (level->m_objectCount) {
                case 0:
                case 65535: {
                    std::string decompressed = cocos2d::ZipUtils::decompressString(
                        level->m_levelString, false, 0
                    );
                    labelContent << fmt::format(
                        "Object Count: ~{}",
                        std::count(decompressed.begin(), decompressed.end(), ';')
                    ) << std::endl;
                    break;
                }
                default:
                    labelContent << fmt::format(
                        "Object Count: {}",
                        level->m_objectCount
                    ) << std::endl;
                    break;
            }
        }

        if (ModManager::Toggles.gameVersion)
            labelContent << fmt::format(
                "Game Version: {}",
                ModManager::GameVersions.count(level->m_gameVersion)
                    ? ModManager::GameVersions.at(level->m_gameVersion)
                    : "Pre-1.7"
                ) << std::endl;

        if (ModManager::Toggles.twoPlayerMode)
            labelContent << fmt::format(
                "2-Player Mode: {}",
                level->m_twoPlayerMode
            ) << std::endl;
        
		if (ModManager::Toggles.levelVersion)
            labelContent << fmt::format(
                "Level Version: {}",
                level->m_levelVersion
            ) << std::endl;
        
		if (ModManager::Toggles.ldmExistence)
            labelContent << fmt::format(
                "LDM Existence: {}",
                level->m_lowDetailMode
            ) << std::endl;
        
        if (ModManager::Toggles.sent) {
            auto cached = ModManager::getLevelFromSentCache(level->m_levelID);

            if (cached.has_value()) {
                labelContent << fmt::format("Sent: {}", cached.value()) << std::endl;
            } else {
                const std::string placeholder = "Sent: loading...";
                labelContent << placeholder << std::endl;
                
                auto req = geode::utils::web::WebRequest();
                auto levelID = static_cast<int>(level->m_levelID);

                // This will call SendDB's API to know if the level was sent or not. The
                // actual sent field will be updated after the request is completed
                m_fields->m_listener.spawn(
                    req.get(fmt::format("https://api.senddb.dev/api/v1/level/{}", levelID)),
                    [self = geode::WeakRef<MyLevelInfoLayer>(this), placeholder, levelID](geode::utils::web::WebResponse res) {
                        auto locked = self.lock();
                        if (!locked || !locked->m_fields->m_label) return;
                        
                        matjson::Value body = res.json().unwrapOrDefault();

                        bool isSent = body.size() > 0 &&
                            body.contains("sends") && body["sends"].size() > 0;
                        if (body.size() > 0)
                            ModManager::addLevelToSentCache(levelID, isSent);

                        std::string labelContent = locked->m_fields->m_label->getString();
                        size_t pos = labelContent.find(placeholder);
                        if (pos != std::string::npos)
                            labelContent.replace(
                                pos,
                                placeholder.length(),
                                fmt::format(
                                    "Sent: {}",
                                    body.size() > 0
                                        ? isSent
                                            ? "true"
                                            : "false"
                                        : "failed"
                                )
                            );

                        locked->m_fields->m_label->setString(labelContent.c_str(), true);
                        locked->updateLayout();
                    }
                );
            }
        }
        
		if (ModManager::Toggles.originalLevel) {
            const int levelID = static_cast<int>(level->m_levelID);
            const int originalLevelID = static_cast<int>(level->m_originalLevel);

            labelContent << fmt::format(
                "Original ID: {}",
                originalLevelID == 0 || levelID == originalLevelID
                    ? "N/A" : std::to_string(originalLevelID)
            ) << std::endl;
        }

        if (ModManager::Toggles.editorTime) {
            const std::chrono::seconds seconds(level->m_workingTime);

            labelContent << fmt::format(
                "Editor: {}h{}m{}s ",
                duration_cast<std::chrono::hours>(seconds).count(),
                duration_cast<std::chrono::minutes>(seconds).count() % 60,
                seconds.count() % 60
            ) << std::endl;
        }

        if (ModManager::Toggles.editorTimeCopies) {
            const std::chrono::seconds seconds(level->m_workingTime + level->m_workingTime2);
            
            labelContent << fmt::format(
                "Edit. (+cop.): {}h{}m{}s ",
                duration_cast<std::chrono::hours>(seconds).count(),
                duration_cast<std::chrono::minutes>(seconds).count() % 60,
                seconds.count() % 60
            ) << std::endl;
        }

        if (ModManager::Toggles.totalAttempts)
            labelContent << fmt::format(
                "Total Attempts: {}",
                static_cast<int>(level->m_attempts)
            ) << std::endl;
        
		if (ModManager::Toggles.totalJumps)
            labelContent << fmt::format(
                "Total Jumps: {}",
                static_cast<int>(level->m_jumps)
            ) << std::endl;
        
		if (ModManager::Toggles.clicks)
            labelContent << fmt::format(
                "Clicks (best att.): {}",
                static_cast<int>(level->m_clicks)
            ) << std::endl;
        
		if (ModManager::Toggles.attemptTime) {
            const std::chrono::seconds seconds(static_cast<int>(level->m_attemptTime));

            labelContent << fmt::format(
                "Time (best att.): {}m{}s",
                duration_cast<std::chrono::minutes>(seconds).count() % 60,
                seconds.count() % 60
            ) << std::endl;
        }

        // Once we got all the values for the label, create it and set its settings
        m_fields->m_label = cocos2d::CCLabelBMFont::create(
            labelContent.str().c_str(),
            "bigFont.fnt"
        );
        m_fields->m_label->setID("level-info-label"_spr);

        m_fields->m_label->setScale(ModManager::Settings.size);
        m_fields->m_label->setOpacity(ModManager::Settings.opacity);
        m_fields->m_label->setColor(ModManager::Settings.color);

        m_fields->m_label->setPosition(m_fields->m_position);

        // And display it
        this->addChild(m_fields->m_label);
		this->updateLayout();

        // After the label's displayed, finally remove the loading circle
        if (m_fields->m_loadingCircle)
            m_fields->m_loadingCircle->fadeOut();
    };
};