#include <Geode/modify/LevelInfoLayer.hpp>

// TODO: Replace "Loading..." text by a Loading Circle

class $modify(LevelInfoLayer) {
    struct Fields {
        int64_t m_textWidthOffset = geode::Mod::get()->getSettingValue<int64_t>("text-width-offset");
        int64_t m_textHeightOffset = geode::Mod::get()->getSettingValue<int64_t>("text-height-offset");
        double m_textSize = geode::Mod::get()->getSettingValue<double>("text-size");
        int64_t m_textOpacity = geode::Mod::get()->getSettingValue<int64_t>("text-opacity");
        cocos2d::ccColor3B m_textColor = geode::Mod::get()->getSettingValue<cocos2d::ccColor3B>("text-color");
        cocos2d::CCSize m_winSize = cocos2d::CCDirector::get()->getWinSize();

        cocos2d::CCLabelBMFont* m_label = cocos2d::CCLabelBMFont::create("Loading...", "bigFont.fnt");
            
        // Get all the settings values
	    bool m_requestedStarsToggle = geode::Mod::get()->getSettingValue<bool>("show-requested-stars");
	    bool m_featuredRankToggle = geode::Mod::get()->getSettingValue<bool>("show-featured-rank");
	    bool m_objectCountToggle = geode::Mod::get()->getSettingValue<bool>("show-object-count");
	    bool m_gameVersionToggle = geode::Mod::get()->getSettingValue<bool>("show-game-version");
	    bool m_levelVersionToggle = geode::Mod::get()->getSettingValue<bool>("show-level-version");
	    bool m_ldmExistence = geode::Mod::get()->getSettingValue<bool>("show-ldm-existence");
	    bool m_originalLevelToggle = geode::Mod::get()->getSettingValue<bool>("show-original-level-id");
	    bool m_twoPlayerModeToggle = geode::Mod::get()->getSettingValue<bool>("show-two-player-mode");
	    bool m_editorTimeToggle = geode::Mod::get()->getSettingValue<bool>("show-editor-time");
	    bool m_editorTimeCopiesToggle = geode::Mod::get()->getSettingValue<bool>("show-editor-time-copies");
	    bool m_totalAttemptsToggle = geode::Mod::get()->getSettingValue<bool>("show-total-attempts");
	    bool m_totalJumpsToggle = geode::Mod::get()->getSettingValue<bool>("show-total-jumps");
	    bool m_clicksToggle = geode::Mod::get()->getSettingValue<bool>("show-clicks");
	    bool m_attemptTimeToggle = geode::Mod::get()->getSettingValue<bool>("show-attempt-time");
    };
    
    bool showInfoLabel() {
        // If all of those values are false, don't display the label text
        if (!m_fields->m_requestedStarsToggle && 
		!m_fields->m_featuredRankToggle && 
		!m_fields->m_objectCountToggle && 
		!m_fields->m_gameVersionToggle && 
		!m_fields->m_levelVersionToggle && 
		!m_fields->m_twoPlayerModeToggle && 
		!m_fields->m_editorTimeToggle && 
		!m_fields->m_editorTimeCopiesToggle && 
		!m_fields->m_totalAttemptsToggle && 
		!m_fields->m_totalJumpsToggle && 
		!m_fields->m_clicksToggle && 
		!m_fields->m_attemptTimeToggle) return false;

        // If any of those is 0, don't display the label text
        else if (m_fields->m_textSize <= 0 || m_fields->m_textOpacity <= 0) return false;

        return true;
    };

    bool init(GJGameLevel* level, bool challenge) {
        if (!LevelInfoLayer::init(level, challenge))
            return false;

        if (!showInfoLabel())
            return true;

        m_fields->m_label->setID("level-info-label"_spr);
        m_fields->m_label->setPositionX(m_fields->m_winSize.width * 0.2 + m_fields->m_textWidthOffset);
        m_fields->m_label->setPositionY(m_fields->m_winSize.height * 0.7 + m_fields->m_textHeightOffset);
        m_fields->m_label->setScale(m_fields->m_textSize);
        m_fields->m_label->setOpacity(static_cast<int64_t>(round(static_cast<double>(m_fields->m_textOpacity) / 100 * 255)));
        m_fields->m_label->setColor(m_fields->m_textColor);
        this->addChild(m_fields->m_label);
        
        // If the level is already downloaded, display the stats
        if (level->m_levelString != "")
            displayLabel(level);

        return true;
    };

    void levelDownloadFinished(GJGameLevel *level) {
		LevelInfoLayer::levelDownloadFinished(level);

		if (showInfoLabel())
            displayLabel(level);
    };

	void displayLabel(GJGameLevel* level) {
        std::stringstream labelString;

        // Here I define every stats, if they are enabled
        if (m_fields->m_requestedStarsToggle)
            labelString << fmt::format("Req. Difficulty: {}\n", level->m_starsRequested);
        
		if (m_fields->m_featuredRankToggle)
            labelString << fmt::format("Featured Rank: {}\n", level->m_featured != 0 ? std::to_string(level->m_featured) : "N/A");

        if (m_fields->m_objectCountToggle) {
            std::string levelString;
            switch(level->m_objectCount) {
                case 0:
                case 65535:
                    levelString = cocos2d::ZipUtils::decompressString(
                        level->m_levelString,
                        false,
                        0
                    );
                    #ifdef __APPLE__
                    labelString << fmt::format("Object Count: ~{}\n", std::count(
                        levelString.begin(),
                        levelString.end(),
                        ';'
                    ));
                    #else
                    labelString << fmt::format("Object Count: ~{}\n", std::ranges::count(
                        levelString.begin(),
                        levelString.end(),
                        ';'
                    ));
                    #endif
                    break;
                default:
                    labelString << fmt::format("Object Count: {}\n", std::to_string(level->m_objectCount));
                    break;
            }
        }

        if (m_fields->m_gameVersionToggle) {
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

        if (m_fields->m_twoPlayerModeToggle)
            labelString << fmt::format("2-Player Mode: {}\n", level->m_twoPlayerMode);
        
		if (m_fields->m_levelVersionToggle)
            labelString << fmt::format("Level Version: {}\n", level->m_levelVersion);
        
		if (m_fields->m_ldmExistence)
            labelString << fmt::format("LDM Existence: {}\n", level->m_lowDetailMode);
        
		if (m_fields->m_originalLevelToggle)
            labelString << fmt::format("Original ID: {}\n", (static_cast<int>(level->m_originalLevel) == static_cast<int>(level->m_levelID) || static_cast<int>(level->m_originalLevel) == 0) ? "N/A" : std::to_string(static_cast<int>(level->m_originalLevel)));

        if (m_fields->m_editorTimeToggle) {
            std::chrono::seconds seconds(level->m_workingTime);
            labelString << fmt::format("Editor: {}h{}m{}s \n", duration_cast<std::chrono::hours>(seconds).count(), duration_cast<std::chrono::minutes>(seconds).count() % 60, seconds.count() % 60);
        }

        if (m_fields->m_editorTimeCopiesToggle) {
            std::chrono::seconds seconds(level->m_workingTime + level->m_workingTime2);
            labelString << fmt::format("Edit. (+cop.): {}h{}m{}s \n", duration_cast<std::chrono::hours>(seconds).count(), duration_cast<std::chrono::minutes>(seconds).count() % 60, seconds.count() % 60);
        }

        if (m_fields->m_totalAttemptsToggle)
            labelString << fmt::format("Total Attempts: {}\n", static_cast<int>(level->m_attempts));
        
		if (m_fields->m_totalJumpsToggle)
            labelString << fmt::format("Total Jumps: {}\n", static_cast<int>(level->m_jumps));
        
		if (m_fields->m_clicksToggle)
            labelString << fmt::format("Clicks (best att.): {}\n", static_cast<int>(level->m_clicks));
        
		if (m_fields->m_attemptTimeToggle) {
            std::chrono::seconds seconds(static_cast<int>(level->m_attemptTime));
            labelString << fmt::format("Time (best att.): {}m{}s\n", duration_cast<std::chrono::minutes>(seconds).count() % 60, seconds.count() % 60);
        }

        // Update Label
        m_fields->m_label->setString(labelString.str().c_str(), true);
		this->updateLayout();

        geode::log::info("Successfully displayed informations about the level '{}' by {}", level->m_levelName, level->m_creatorName);
    }
};