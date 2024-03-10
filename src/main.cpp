#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/Bindings.hpp>
#include <Geode/cocos/support/zip_support/ZipUtils.h>

using namespace geode::prelude;
using namespace std::chrono;

class $modify(LevelInfoLayer) {
	bool init(GJGameLevel* level, bool p1) {
		if (!LevelInfoLayer::init(level, false))
			return false;

		// Get all the settings values
		bool requestedStarsToggle = Mod::get()->getSettingValue<bool>("show-requested-stars");
		bool featuredRankToggle = Mod::get()->getSettingValue<bool>("show-featured-rank");
		bool objectCountToggle = Mod::get()->getSettingValue<bool>("show-object-count");
		bool gameVersionToggle = Mod::get()->getSettingValue<bool>("show-game-version");
		bool levelVersionToggle = Mod::get()->getSettingValue<bool>("show-level-version");
		bool twoPlayerModeToggle = Mod::get()->getSettingValue<bool>("show-two-player-mode");
		bool editorTimeToggle = Mod::get()->getSettingValue<bool>("show-editor-time");
		bool editorTimeCopiesToggle = Mod::get()->getSettingValue<bool>("show-editor-time-copies");
		bool totalAttemptsToggle = Mod::get()->getSettingValue<bool>("show-total-attempts");
		bool totalJumpsToggle = Mod::get()->getSettingValue<bool>("show-total-jumps");
		bool clicksToggle = Mod::get()->getSettingValue<bool>("show-clicks");
		bool attemptTimeToggle = Mod::get()->getSettingValue<bool>("show-attempt-time");

		// Only get the corresponding values and display them if atleast one of the settings are true
		if (requestedStarsToggle || featuredRankToggle || objectCountToggle || gameVersionToggle || levelVersionToggle || twoPlayerModeToggle || editorTimeToggle || editorTimeCopiesToggle || totalAttemptsToggle || totalJumpsToggle || clicksToggle || attemptTimeToggle) {
			// Get the text color defined by the user
			auto textColor = Mod::get()->getSettingValue<cocos2d::ccColor3B>("text-color");
			
			// Get the size of the window
        	const auto winSize = CCDirector::get()->getWinSize();

			std::string labelText;

			// Here I define every stats, if they are enabled
			if (requestedStarsToggle) {
    			labelText += fmt::format("Req. Difficulty: {}\n", level->m_starsRequested).c_str();
			}

			if (featuredRankToggle) {
				labelText += fmt::format("Featured Rank: {}\n", (level -> m_featured != 0) ? std::to_string(level -> m_featured) : "N/A").c_str();
			}

			if (objectCountToggle) {
				std::string levelString;
				size_t objectCount = static_cast<int>(level->m_objectCount);
				switch(level->m_objectCount) {
					case 0:
					case 65535:
						levelString = ZipUtils::decompressString(level->m_levelString, false, 0);
						objectCount = 0;
						objectCount = count(levelString.begin(), levelString.end(), ';');
						labelText += fmt::format("Object Count: ~{}\n", ((objectCount >= 0) ? std::to_string(objectCount) : "Unknown")).c_str();
						break;
					default:
						labelText += fmt::format("Object Count: {}\n", ((objectCount >= 0) ? std::to_string(objectCount) : "Unknown")).c_str();
						break;
				}
			}
				
			if (gameVersionToggle) {
				switch(level -> m_gameVersion) {
					case 22 :
						labelText += fmt::format("Game Version: 2.2\n").c_str();
						break;

					case 21 :
						labelText += fmt::format("Game Version: 2.1\n").c_str();
						break;

					case 20 :
						labelText += fmt::format("Game Version: 2.0\n").c_str();
						break;

					case 19 :
						labelText += fmt::format("Game Version: 1.9\n").c_str();
						break;

					case 18 :
						labelText += fmt::format("Game Version: 1.8\n").c_str();
						break;

					case 10 :
						labelText += fmt::format("Game Version: 1.7\n").c_str();
						break;

					default :
						labelText += fmt::format("Game Version: Pre-1.7\n").c_str();
						break;
				}
			}

			if (twoPlayerModeToggle) {
				labelText += fmt::format("2 Player Mode: {}\n", level->m_twoPlayerMode).c_str();
			}

			if (levelVersionToggle) {
				labelText += fmt::format("Level Version: {}\n", level->m_levelVersion).c_str();
			}

			if (editorTimeToggle) {
				seconds seconds(level->m_workingTime);
				labelText += fmt::format("Edit. Time: {}h{}m{}s \n", duration_cast<hours>(seconds).count(), duration_cast<minutes>(seconds).count() % 60, seconds.count() % 60).c_str();
			}

			if (editorTimeCopiesToggle) {
				seconds seconds(level->m_workingTime + level->m_workingTime2);
				labelText += fmt::format("Edit. Time (+cop.): {}h{}m{}s \n", duration_cast<hours>(seconds).count(), duration_cast<minutes>(seconds).count() % 60, seconds.count() % 60).c_str();
			}

			if (totalAttemptsToggle) {
				int attempts = static_cast<int>(level->m_attempts);
				labelText += fmt::format("Total Attempts: {}\n", attempts).c_str();
			}

			if (totalJumpsToggle) {
				int jumps = static_cast<int>(level->m_jumps);
				labelText += fmt::format("Total Jumps: {}\n", jumps).c_str();
			}

			if (clicksToggle) {
				int clicks = static_cast<int>(level->m_clicks);
				labelText += fmt::format("Clicks (best att.): {}\n", clicks).c_str();
			}

			if (attemptTimeToggle) {
				int attempt_time = static_cast<int>(level->m_attemptTime);
				seconds seconds(attempt_time);
				labelText += fmt::format("Time (best att.): {}m{}s\n", duration_cast<minutes>(seconds).count() % 60, seconds.count() % 60).c_str();
			}

			// Display the label
        	auto label = CCLabelBMFont::create(fmt::format("{}", labelText).c_str(), "bigFont.fnt");
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
