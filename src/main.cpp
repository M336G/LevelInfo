#include <Geode/modify/LevelInfoLayer.hpp>

#include "LevelInfoSettingsManager.hpp"

class $modify(LevelInfoLayer) {
    public:
        struct Fields {
            cocos2d::CCLabelBMFont* label = cocos2d::CCLabelBMFont::create("Loading...", "bigFont.fnt");
        };

    bool init(GJGameLevel* level, bool p1) {
        if (!LevelInfoLayer::init(level, p1))
            return false;

        // If all of those values are false, don't display the label text
        if (!LevelInfoSettingsManager::requestedStarsToggle && 
		!LevelInfoSettingsManager::featuredRankToggle && 
		!LevelInfoSettingsManager::objectCountToggle && 
		!LevelInfoSettingsManager::gameVersionToggle && 
		!LevelInfoSettingsManager::levelVersionToggle && 
		!LevelInfoSettingsManager::twoPlayerModeToggle && 
		!LevelInfoSettingsManager::editorTimeToggle && 
		!LevelInfoSettingsManager::editorTimeCopiesToggle && 
		!LevelInfoSettingsManager::totalAttemptsToggle && 
		!LevelInfoSettingsManager::totalJumpsToggle && 
		!LevelInfoSettingsManager::clicksToggle && 
		!LevelInfoSettingsManager::attemptTimeToggle) return true;

        // If any of those is 0, don't display the label text
        if (LevelInfoSettingsManager::textSize <= 0) return true;

        if (LevelInfoSettingsManager::textOpacity <= 0) return true;

		// Display label text otherwise
        m_fields->label->setID("level-info-label"_spr);
        m_fields->label->setPosition(LevelInfoSettingsManager::winSize / 1.4);
        m_fields->label->setPositionX(110);
        m_fields->label->setScale(LevelInfoSettingsManager::textSize);
        m_fields->label->setOpacity(static_cast<int64_t>(round(static_cast<double>(LevelInfoSettingsManager::textOpacity) / 100 * 255)));
        m_fields->label->setColor(LevelInfoSettingsManager::textColor);
        this->addChild(m_fields->label);

        // If the level is already downloaded, display the stats
        /*
            TODO: Find ways to:
            - If downloaded + loaded: show stats directly (done)
            - If downloaded but not loaded: show "Loading..." then show stats
            - If not downloaded: show "Loading..." then show stats (done)
        */
        if (this->m_playSprite->isVisible()) displayLabel(level);

        return true;
    };

    void levelDownloadFinished(GJGameLevel *level) {
		LevelInfoLayer::levelDownloadFinished(level);

		displayLabel(level);
		
    };

	void displayLabel(GJGameLevel* level) {
        std::stringstream labelString;

        // Here I define every stats, if they are enabled
        if (LevelInfoSettingsManager::requestedStarsToggle) labelString << fmt::format("Req. Difficulty: {}\n", level->m_starsRequested);
        
		if (LevelInfoSettingsManager::featuredRankToggle) labelString << fmt::format("Featured Rank: {}\n", level->m_featured != 0 ? std::to_string(level->m_featured) : "N/A");

        if (LevelInfoSettingsManager::objectCountToggle) {
            std::string levelString;
            switch(level->m_objectCount) {
                case 0:
                case 65535:
                    levelString = cocos2d::ZipUtils::decompressString(level->m_levelString, false, 0);
                    #ifdef __APPLE__
                    labelString << fmt::format("Object Count: ~{}\n", std::count(levelString.begin(), levelString.end(), ';'));
                    #else
                    labelString << fmt::format("Object Count: ~{}\n", std::ranges::count(levelString.begin(), levelString.end(), ';'));
                    #endif
                    break;
                default:
                    labelString << fmt::format("Object Count: {}\n", std::to_string(level->m_objectCount));
                    break;
            }
        }

        if (LevelInfoSettingsManager::gameVersionToggle) {
            switch(level->m_gameVersion) {
                case 22:
                    labelString << "Game Version: 2.2\n";
                    break;
                case 21:
                    labelString << "Game Version: 2.1\n";
                    break;
                case 20:
                    labelString << "Game Version: 2.0\n";
                    break;
                case 19:
                    labelString << "Game Version: 1.9\n";
                    break;
                case 18:
                    labelString << "Game Version: 1.8\n";
                    break;
                case 10:
                    labelString << "Game Version: 1.7\n";
                    break;
                default:
                    labelString << "Game Version: Pre-1.7\n";
                    break;
            }
        }

        if (LevelInfoSettingsManager::twoPlayerModeToggle) labelString << fmt::format("2-Player Mode: {}\n", level->m_twoPlayerMode);
        
		if (LevelInfoSettingsManager::levelVersionToggle) labelString << fmt::format("Level Version: {}\n", level->m_levelVersion);
        
		if (LevelInfoSettingsManager::ldmExistence) labelString << fmt::format("LDM Existence: {}\n", level->m_lowDetailMode);
        
		if (LevelInfoSettingsManager::originalLevelToggle) labelString << fmt::format("Original ID: {}\n", (static_cast<int>(level->m_originalLevel) == static_cast<int>(level->m_levelID) || static_cast<int>(level->m_originalLevel) == 0) ? "N/A" : std::to_string(static_cast<int>(level->m_originalLevel)));

        if (LevelInfoSettingsManager::editorTimeToggle) {
            std::chrono::seconds seconds(level->m_workingTime);
            labelString << fmt::format("Editor: {}h{}m{}s \n", duration_cast<std::chrono::hours>(seconds).count(), duration_cast<std::chrono::minutes>(seconds).count() % 60, seconds.count() % 60);
        }

        if (LevelInfoSettingsManager::editorTimeCopiesToggle) {
            std::chrono::seconds seconds(level->m_workingTime + level->m_workingTime2);
            labelString << fmt::format("Edit. (+cop.): {}h{}m{}s \n", duration_cast<std::chrono::hours>(seconds).count(), duration_cast<std::chrono::minutes>(seconds).count() % 60, seconds.count() % 60);
        }

        if (LevelInfoSettingsManager::totalAttemptsToggle) labelString << fmt::format("Total Attempts: {}\n", static_cast<int>(level->m_attempts));
        
		if (LevelInfoSettingsManager::totalJumpsToggle) labelString << fmt::format("Total Jumps: {}\n", static_cast<int>(level->m_jumps));
        
		if (LevelInfoSettingsManager::clicksToggle) labelString << fmt::format("Clicks (best att.): {}\n", static_cast<int>(level->m_clicks));
        
		if (LevelInfoSettingsManager::attemptTimeToggle) {
            std::chrono::seconds seconds(static_cast<int>(level->m_attemptTime));
            labelString << fmt::format("Time (best att.): {}m{}s\n", duration_cast<std::chrono::minutes>(seconds).count() % 60, seconds.count() % 60);
        }

        m_fields->label->setString(labelString.str().c_str(), true);
		this->updateLayout();

        geode::log::info("Successfully displayed informations about the level '{}' by {}", level->m_levelName, level->m_creatorName);
    }
};