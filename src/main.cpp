#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/Bindings.hpp>

using namespace geode::prelude;

class $modify(LevelInfoLayer) {
	bool init(GJGameLevel* level, bool p1) {
		if (!LevelInfoLayer::init(level, false))
			return false;

		std::string labelText;

		// Get all the settings values
		auto requestedStarsToggle = Mod::get()->getSettingValue<bool>("show-requested-stars");
		auto featuredRankToggle = Mod::get()->getSettingValue<bool>("show-featured-rank");
		auto objectCountToggle = Mod::get()->getSettingValue<bool>("show-object-count");
		auto gameVersionToggle = Mod::get()->getSettingValue<bool>("show-game-version");
		auto levelVersionToggle = Mod::get()->getSettingValue<bool>("show-level-version");
		auto twoPlayerModeToggle = Mod::get()->getSettingValue<bool>("show-two-player-mode");

		// Get the text color defined by the user
		auto textColor = Mod::get()->getSettingValue<cocos2d::ccColor3B>("text-color");

		// Get the size of the window
        const auto winSize = CCDirector::get()->getWinSize();

		// Here I define every stats, if they are enabled
	if (requestedStarsToggle) {
    	labelText += fmt::format("Requested Difficulty: {}\n", level->m_starsRequested).c_str();
	}

		if (featuredRankToggle) {
			if (level->m_featured != 0) {
				labelText += fmt::format("Featured Rank: {}\n", level->m_featured).c_str();
			}
			else {
				// If it's 0 (not featured), I display "N/A" instead
				labelText += fmt::format("Featured Rank: N/A\n").c_str();
			}
			
		}

		if (objectCountToggle) {
			int objectCount = static_cast<int>(level->m_objectCount);

			if (objectCount != 0) {
				labelText += fmt::format("Object Count: {}\n", objectCount).c_str();
			}
			else {
				// same thing
				labelText += fmt::format("Object Count: Unknown\n").c_str();
			}
		}

		if (gameVersionToggle) {
			if (level->m_gameVersion == 22) {
				labelText += fmt::format("Game Version: 2.2\n").c_str();
			}
			else if (level->m_gameVersion == 21) {
				labelText += fmt::format("Game Version: 2.1\n").c_str();
			}
			else if (level->m_gameVersion == 20) {
				labelText += fmt::format("Game Version: 2.0\n").c_str();
			}
			else if (level->m_gameVersion == 19) {
				labelText += fmt::format("Game Version: 1.9\n").c_str();
			}
			else if (level->m_gameVersion == 18) {
				labelText += fmt::format("Game Version: 1.8\n").c_str();
			}
			else if (level->m_gameVersion == 10) {
				labelText += fmt::format("Game Version: 1.7\n").c_str();
			}
			else {
				labelText += fmt::format("Game Version: Unknown\n").c_str();
			}
		}

		if (twoPlayerModeToggle) {
			labelText += fmt::format("2 Player Mode: {}\n", level->m_twoPlayerMode).c_str();
		}

		if (levelVersionToggle) {
			labelText += fmt::format("Level Version: {}\n", level->m_levelVersion).c_str();
		}

		// Try to display the label
		try {
        	auto label = CCLabelBMFont::create(fmt::format("{}", labelText).c_str(), "bigFont.fnt");
        	label->setPosition(winSize / 1.4);
			label->setPositionX(100);
			label->setScale(0.3);
			label->setColor(textColor);
        	this->addChild(label);
			geode::log::debug("Successfully displayed the level info text");
		}

		// If it cannot, it throws an error in the logs and in-game
		catch (...) {
			Notification::create("An error occured while trying to display the level info text", NotificationIcon::Error, 1)->show();
			geode::log::error("An error occured while trying to display the level info text");
		}

		return true;
	}
};