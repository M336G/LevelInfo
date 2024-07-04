#pragma once

#include <Geode/Geode.hpp>

class LevelInfoSettingsManager {
public:
    // Get all the settings values
	inline static const bool requestedStarsToggle = geode::Mod::get()->getSettingValue<bool>("show-requested-stars");
	inline static const bool featuredRankToggle = geode::Mod::get()->getSettingValue<bool>("show-featured-rank");
	inline static const bool objectCountToggle = geode::Mod::get()->getSettingValue<bool>("show-object-count");
	inline static const bool gameVersionToggle = geode::Mod::get()->getSettingValue<bool>("show-game-version");
	inline static const bool levelVersionToggle = geode::Mod::get()->getSettingValue<bool>("show-level-version");
	inline static const bool ldmExistence = geode::Mod::get()->getSettingValue<bool>("show-ldm-existence");
	inline static const bool originalLevelToggle = geode::Mod::get()->getSettingValue<bool>("show-original-level-id");
	inline static const bool twoPlayerModeToggle = geode::Mod::get()->getSettingValue<bool>("show-two-player-mode");
	inline static const bool editorTimeToggle = geode::Mod::get()->getSettingValue<bool>("show-editor-time");
	inline static const bool editorTimeCopiesToggle = geode::Mod::get()->getSettingValue<bool>("show-editor-time-copies");
	inline static const bool totalAttemptsToggle = geode::Mod::get()->getSettingValue<bool>("show-total-attempts");
	inline static const bool totalJumpsToggle = geode::Mod::get()->getSettingValue<bool>("show-total-jumps");
	inline static const bool clicksToggle = geode::Mod::get()->getSettingValue<bool>("show-clicks");
	inline static const bool attemptTimeToggle = geode::Mod::get()->getSettingValue<bool>("show-attempt-time");

    inline static const double textSize = geode::Mod::get()->getSettingValue<double>("text-size");
    inline static const int64_t textOpacity = geode::Mod::get()->getSettingValue<int64_t>("text-opacity");
    inline static const cocos2d::ccColor3B textColor = geode::Mod::get()->getSettingValue<cocos2d::ccColor3B>("text-color");
    inline static const cocos2d::CCSize winSize = cocos2d::CCDirector::get()->getWinSize();
};