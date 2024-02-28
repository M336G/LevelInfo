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
		auto levelVersionToggle = Mod::get()->getSettingValue<bool>("show-level-version");
		auto twoPlayerModeToggle = Mod::get()->getSettingValue<bool>("show-two-player-mode");

		// Get the text color defined by the user
		auto textColor = Mod::get()->getSettingValue<cocos2d::ccColor3B>("text-color");

		// Get the size of the window
        auto winSize = CCDirector::get()->getWinSize();

		// Here I define every stats, if they are enabled
	if (requestedStarsToggle != false) {
    	labelText += fmt::format("Requested Difficulty: {}\n", level->m_starsRequested).c_str();
	}

		if (featuredRankToggle != false) {
			if (level->m_featured != 0) {
				labelText += fmt::format("Featured Rank: {}\n", level->m_featured).c_str();
			}
			else {
				// If it's 0 (not featured), I display "N/A" instead
				labelText += fmt::format("Featured Rank: N/A\n").c_str();
			}
			
		}

		if (objectCountToggle != false) {
			int objectCount = static_cast<int>(level->m_objectCount);
			labelText += fmt::format("Object Count: {}\n", objectCount).c_str();
		}

		if (twoPlayerModeToggle != false) {
			labelText += fmt::format("2 Player Mode: {}\n", level->m_twoPlayerMode).c_str();
		}

		if (levelVersionToggle != false) {
			labelText += fmt::format("Level Version: {}\n", level->m_levelVersion).c_str();
		}

		// Display the label
        auto label = CCLabelBMFont::create(fmt::format("{}", labelText).c_str(), "bigFont.fnt");
        label->setPosition(winSize / 1.4);
		label->setPositionX(100);
		label->setScale(0.3);
		label->setColor(textColor);
        this->addChild(label);

		return true;
	}
};