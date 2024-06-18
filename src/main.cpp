#include <Geode/modify/LevelInfoLayer.hpp>

class $modify(LevelInfoLayer) {
	bool init(GJGameLevel* level, bool p1) {
		if (!LevelInfoLayer::init(level, p1))
			return false;

		// Get all the settings values
		const auto requestedStarsToggle = geode::Mod::get()->getSettingValue<bool>("show-requested-stars");
		const auto featuredRankToggle = geode::Mod::get()->getSettingValue<bool>("show-featured-rank");
		const auto objectCountToggle = geode::Mod::get()->getSettingValue<bool>("show-object-count");
		const auto gameVersionToggle = geode::Mod::get()->getSettingValue<bool>("show-game-version");
		const auto levelVersionToggle = geode::Mod::get()->getSettingValue<bool>("show-level-version");
		const auto originalLevelToggle = geode::Mod::get()->getSettingValue<bool>("show-original-level-id");
		const auto twoPlayerModeToggle = geode::Mod::get()->getSettingValue<bool>("show-two-player-mode");
		const auto editorTimeToggle = geode::Mod::get()->getSettingValue<bool>("show-editor-time");
		const auto editorTimeCopiesToggle = geode::Mod::get()->getSettingValue<bool>("show-editor-time-copies");
		const auto totalAttemptsToggle = geode::Mod::get()->getSettingValue<bool>("show-total-attempts");
		const auto totalJumpsToggle = geode::Mod::get()->getSettingValue<bool>("show-total-jumps");
		const auto clicksToggle = geode::Mod::get()->getSettingValue<bool>("show-clicks");
		const auto attemptTimeToggle = geode::Mod::get()->getSettingValue<bool>("show-attempt-time");

		// If all of those values are false, don't display the label text
		if (!requestedStarsToggle && !featuredRankToggle && !objectCountToggle && !gameVersionToggle && !levelVersionToggle && !twoPlayerModeToggle && !editorTimeToggle && !editorTimeCopiesToggle && !totalAttemptsToggle && !totalJumpsToggle && !clicksToggle && !attemptTimeToggle) return true;

		const auto textSize = geode::Mod::get()->getSettingValue<double>("text-size");
		if (textSize <= 0) return true;

		const auto textOpacity = geode::Mod::get()->getSettingValue<int64_t>("text-opacity");
		if (textOpacity <= 0) return true;

		const auto textColor = geode::Mod::get()->getSettingValue<cocos2d::ccColor3B>("text-color");

		// Get the size of the window
		const auto winSize = cocos2d::CCDirector::get()->getWinSize();

		std::stringstream labelString;
			
		// Here I define every stats, if they are enabled
		if (requestedStarsToggle) labelString << fmt::format("Req. Difficulty: {}\n", level->m_starsRequested);

		if (featuredRankToggle) labelString << fmt::format("Featured Rank: {}\n", level->m_featured != 0 ? std::to_string(level->m_featured) : "N/A");

		if (objectCountToggle) {
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

		if (gameVersionToggle) {
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

		if (twoPlayerModeToggle) labelString << fmt::format("2-Player Mode: {}\n", level->m_twoPlayerMode);

		if (levelVersionToggle) labelString << fmt::format("Level Version: {}\n", level->m_levelVersion);
		// Yes.
		if (originalLevelToggle) labelString << fmt::format("Original ID: {}\n", (static_cast<int>(level->m_originalLevel) == static_cast<int>(level->m_levelID) || static_cast<int>(level->m_originalLevel) == 0) ? "N/A" : std::to_string(static_cast<int>(level->m_originalLevel)));

		if (editorTimeToggle) {
			std::chrono::seconds seconds(level->m_workingTime);
			labelString << fmt::format("Editor: {}h{}m{}s \n", duration_cast<std::chrono::hours>(seconds).count(), duration_cast<std::chrono::minutes>(seconds).count() % 60, seconds.count() % 60);
		}

		if (editorTimeCopiesToggle) {
			std::chrono::seconds seconds(level->m_workingTime + level->m_workingTime2);
			labelString << fmt::format("Edit. (+cop.): {}h{}m{}s \n", duration_cast<std::chrono::hours>(seconds).count(), duration_cast<std::chrono::minutes>(seconds).count() % 60, seconds.count() % 60);
		}

		if (totalAttemptsToggle) labelString << fmt::format("Total Attempts: {}\n", static_cast<int>(level->m_attempts));

		if (totalJumpsToggle) labelString << fmt::format("Total Jumps: {}\n", static_cast<int>(level->m_jumps));

		if (clicksToggle) labelString << fmt::format("Clicks (best att.): {}\n", static_cast<int>(level->m_clicks));

		if (attemptTimeToggle) {
			std::chrono::seconds seconds(static_cast<int>(level->m_attemptTime));
			labelString << fmt::format("Time (best att.): {}m{}s\n", duration_cast<std::chrono::minutes>(seconds).count() % 60, seconds.count() % 60);
		}

		// Display the label
		const auto label = cocos2d::CCLabelBMFont::create(labelString.str().c_str(), "bigFont.fnt");
		label->setID("level-info-label"_spr);
		label->setPosition(winSize / 1.4);
		label->setPositionX(110);
		label->setScale(textSize);
		label->setOpacity(static_cast<int64_t>(round(static_cast<double>(textOpacity) / 100 * 255)));
		label->setColor(textColor);
		this->addChild(label);
		geode::log::info("Successfully displayed informations about the level '{}' by {}", level->m_levelName, level->m_creatorName);

		return true;
	}
};