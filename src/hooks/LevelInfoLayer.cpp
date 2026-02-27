#include <Geode/modify/LevelInfoLayer.hpp>
#include <cue/LoadingCircle.hpp> // This may be overkill

#include "../managers/SettingsManager.h"
#include "../managers/SentCacheManager.h"
#include "../utils/Utils.h"

class $modify(MyLevelInfoLayer, LevelInfoLayer) {
    struct Fields {
        cue::LoadingCircle *m_loadingCircle = cue::LoadingCircle::create();
        cocos2d::CCLabelBMFont *m_label;

        cocos2d::CCPoint m_position = {
            SettingsManager::Display.getWidth(),
            SettingsManager::Display.getHeight()
        };

        geode::async::TaskHolder<geode::utils::web::WebResponse> m_listener;
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
    void levelDownloadFinished(GJGameLevel *level) {
		LevelInfoLayer::levelDownloadFinished(level);

        if (SettingsManager::Display.size > 0 && SettingsManager::Toggles.anyEnabled())
            showLevelInfo(level);
    };

	void showLevelInfo(GJGameLevel* level) {
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
                    std::string decompressed = cocos2d::ZipUtils::decompressString(
                        level->m_levelString, false, 0
                    );
                    labelContent << "Object Count: ~" <<
                        Utils::FormatNumber(std::count(decompressed.begin(), decompressed.end(), ';'))
                        << std::endl;
                    break;
                }
                default:
                    labelContent << "Object Count: " << Utils::FormatNumber(level->m_objectCount)
                        << std::endl;
                    break;
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
        
        if (SettingsManager::Toggles.sent && level->m_stars == 0) {
            auto cached = SentCacheManager::GetLevel(level->m_levelID);

            if (cached) {
                labelContent << "Sent: " << (cached.value() ? "Yes" : "No") << std::endl;
            } else {
                const std::string placeholder = "Sent: Loading...";
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
                            SentCacheManager::SaveLevel(levelID, isSent);

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
                                            ? "Yes"
                                            : "No"
                                        : "Failed"
                                )
                            );

                        locked->m_fields->m_label->setString(labelContent.c_str(), true);
                        locked->updateLayout();
                    }
                );
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

        if (SettingsManager::Toggles.totalAttempts)
            labelContent << "Total Attempts: " << Utils::FormatNumber(level->m_attempts)
                << std::endl;
        
		if (SettingsManager::Toggles.totalJumps)
            labelContent << "Total Jumps: " << Utils::FormatNumber(level->m_jumps)
                << std::endl;
        
		if (SettingsManager::Toggles.clicks)
            labelContent << "Clicks (best att.): " << Utils::FormatNumber(level->m_clicks)
                << std::endl;
        
		if (SettingsManager::Toggles.attemptTime)
            labelContent << "Time (best att.): " <<
                Utils::FormatTime(std::chrono::seconds(static_cast<int>(level->m_attemptTime)))
                << std::endl;

        // Once we got all the values for the label, create it and set its settings
        m_fields->m_label = cocos2d::CCLabelBMFont::create(
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