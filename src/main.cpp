#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/cocos/support/zip_support/ZipUtils.h>

using namespace geode::prelude;
using namespace std::chrono;

class $modify(LevelInfoLayer) {
	bool init(GJGameLevel* level, bool p1) {
		if (!LevelInfoLayer::init(level, false))
			return false;

		// Get all the settings values
		auto requestedStarsToggle = Mod::get()->getSettingValue<bool>("show-requested-stars");
		auto featuredRankToggle = Mod::get()->getSettingValue<bool>("show-featured-rank");
		auto objectCountToggle = Mod::get()->getSettingValue<bool>("show-object-count");
		auto gameVersionToggle = Mod::get()->getSettingValue<bool>("show-game-version");
		auto levelVersionToggle = Mod::get()->getSettingValue<bool>("show-level-version");
		auto twoPlayerModeToggle = Mod::get()->getSettingValue<bool>("show-two-player-mode");
		auto editorTimeToggle = Mod::get()->getSettingValue<bool>("show-editor-time");
		auto editorTimeCopiesToggle = Mod::get()->getSettingValue<bool>("show-editor-time-copies");
		auto totalAttemptsToggle = Mod::get()->getSettingValue<bool>("show-total-attempts");
		auto totalJumpsToggle = Mod::get()->getSettingValue<bool>("show-total-jumps");
		auto clicksToggle = Mod::get()->getSettingValue<bool>("show-clicks");
		auto attemptTimeToggle = Mod::get()->getSettingValue<bool>("show-attempt-time");

		// Only get the corresponding values and display them if atleast one of the settings are true
		if (requestedStarsToggle || featuredRankToggle || objectCountToggle || gameVersionToggle || levelVersionToggle || twoPlayerModeToggle || editorTimeToggle || editorTimeCopiesToggle || totalAttemptsToggle || totalJumpsToggle || clicksToggle || attemptTimeToggle) {
			// Get the text color defined by the user
			auto textColor = Mod::get()->getSettingValue<cocos2d::ccColor3B>("text-color");
			
			// Get the size of the window
        	const auto winSize = CCDirector::get()->getWinSize();

			std::stringstream labelStringStream;
			
			// Here I define every stats, if they are enabled
			if (requestedStarsToggle) {
				 labelStringStream << fmt::format("Req. Difficulty: {}\n", level->m_starsRequested);
			}

			if (featuredRankToggle) {
				labelStringStream << fmt::format("Featured Rank: {}\n", (level -> m_featured != 0) ? std::to_string(level -> m_featured) : "N/A");
			}

			if (objectCountToggle) {
    			std::string levelString;
    			size_t objectCount = static_cast<size_t>(level->m_objectCount);
    			std::string objectCountStr;
    			switch(objectCount) {
        			case 0:
        			case 65535:
            			levelString = ZipUtils::decompressString(level->m_levelString, false, 0);
            			objectCount = count(levelString.begin(), levelString.end(), ';');
            			objectCountStr = std::to_string(objectCount);
            			labelStringStream << fmt::format("Object Count: ~{}\n", objectCountStr);
            			break;
        			default:
            			objectCountStr = std::to_string(objectCount);
            			labelStringStream << fmt::format("Object Count: {}\n", objectCountStr);
            			break;
    			}
			}
				
			if (gameVersionToggle) {
				switch(level -> m_gameVersion) {
					case 22:
						labelStringStream << fmt::format("Game Version: 2.2\n");
						break;
					case 21:
						labelStringStream << fmt::format("Game Version: 2.1\n");
						break;
					case 20:
						labelStringStream << fmt::format("Game Version: 2.0\n");
						break;
					case 19:
						labelStringStream << fmt::format("Game Version: 1.9\n");
						break;
					case 18:
						labelStringStream << fmt::format("Game Version: 1.8\n");
						break;
					case 10:
						labelStringStream << fmt::format("Game Version: 1.7\n");
						break;
					default:
						labelStringStream << fmt::format("Game Version: Pre-1.7\n");
						break;
				}
			}	

			if (twoPlayerModeToggle) {
				labelStringStream << fmt::format("2 Player Mode: {}\n", level->m_twoPlayerMode);
			}

			if (levelVersionToggle) {
				labelStringStream << fmt::format("Level Version: {}\n", level->m_levelVersion);
			}

			if (editorTimeToggle) {
				seconds seconds(level->m_workingTime);
				labelStringStream << fmt::format("Edit. Time: {}h{}m{}s \n", duration_cast<hours>(seconds).count(), duration_cast<minutes>(seconds).count() % 60, seconds.count() % 60);
			}

			if (editorTimeCopiesToggle) {
				seconds seconds(level->m_workingTime + level->m_workingTime2);
				labelStringStream << fmt::format("Edit. Time (+cop.): {}h{}m{}s \n", duration_cast<hours>(seconds).count(), duration_cast<minutes>(seconds).count() % 60, seconds.count() % 60);
			}

			if (totalAttemptsToggle) {
				int attempts = static_cast<int>(level->m_attempts);
				labelStringStream << fmt::format("Total Attempts: {}\n", attempts);
			}

			if (totalJumpsToggle) {
				int jumps = static_cast<int>(level->m_jumps);
				labelStringStream << fmt::format("Total Jumps: {}\n", jumps);
			}

			if (clicksToggle) {
				int clicks = static_cast<int>(level->m_clicks);
				labelStringStream << fmt::format("Clicks (best att.): {}\n", clicks);
			}

			if (attemptTimeToggle) {
				int attempt_time = static_cast<int>(level->m_attemptTime);
				seconds seconds(attempt_time);
				labelStringStream << fmt::format("Time (best att.): {}m{}s\n", duration_cast<minutes>(seconds).count() % 60, seconds.count() % 60);
			}

			std::string labelText = labelStringStream.str();

			// Display the label
        	auto label = CCLabelBMFont::create(labelText.c_str(), "bigFont.fnt");
			label->setID("level-info-label");
        	label->setPosition(winSize / 1.4);
			label->setPositionX(100);
			label->setScale(0.25);
			label->setColor(textColor);
        	this->addChild(label);
			geode::log::info("Successfully displayed informations about the level '{}' by {}", level->m_levelName, level->m_creatorName);
		}	

		return true;
	}
};
