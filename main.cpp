#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/binding/GJGameLevel.hpp>
#include <Geode/Bindings.hpp>

using namespace geode::prelude;

class $modify(LevelInfoLayer) {
	bool init(GJGameLevel* level, bool p1) {

		std::string labelText;

		// Get all the settings values
		bool requestedStarsToggle = Mod::get()->getSettingValue<bool>("show-requested-stars");
		bool featuredRankToggle = Mod::get()->getSettingValue<bool>("show-featured-rank");
		bool objectCountToggle = Mod::get()->getSettingValue<bool>("show-object-count");
		bool gameVersionToggle = Mod::get()->getSettingValue<bool>("show-game-version");
		bool levelVersionToggle = Mod::get()->getSettingValue<bool>("show-level-version");
		bool twoPlayerModeToggle = Mod::get()->getSettingValue<bool>("show-two-player-mode");

		// Get the text color defined by the user
		auto textColor = Mod::get()->getSettingValue<cocos2d::ccColor3B>("text-color");

		// Get the size of the window
        const auto winSize = CCDirector::get()->getWinSize();

		// Here I define every stats, if they are enabled
		if (requestedStarsToggle) {
    		labelText += fmt::format("Requested Difficulty: {}\n", level->m_starsRequested).c_str();
		}

		if (featuredRankToggle) {
			labelText += fmt::format("Featured Rank: {}\n", (level -> m_featured != 0) ? std::to_string(level -> m_featured) : "N/A").c_str();
		}

		if (objectCountToggle) {
			int objectCount = static_cast<int>(level->m_objectCount);

			labelText += fmt::format("Object Count: {}\n", ((objectCount >= 0) ? std::to_string(objectCount) : "Unknown")).c_str();
		}

		if (gameVersionToggle)
		{
			switch(level -> m_gameVersion)
			{
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