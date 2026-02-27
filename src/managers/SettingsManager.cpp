#include "SettingsManager.h"
#include "SentCacheManager.h"

geode::Mod *SettingsManager::Mod = geode::Mod::get();

// First initialize all the parameters
CustomStruct::DisplaySettings SettingsManager::Display = {
    static_cast<int>(Mod->getSettingValue<int64_t>("text-width-offset")),
    static_cast<int>(Mod->getSettingValue<int64_t>("text-height-offset")),
    static_cast<float>(Mod->getSettingValue<double>("text-size")),
    static_cast<int>(round(static_cast<double>(Mod->getSettingValue<int64_t>("text-opacity")) / 100 * 255)),
    SettingsManager::Mod->getSettingValue<cocos2d::ccColor3B>("text-color"),
    SettingsManager::Mod->getSettingValue<std::string>("number-separator")
};

CustomStruct::ToggleSettings SettingsManager::Toggles = {
    SettingsManager::Mod->getSettingValue<bool>("show-requested-stars"),
    SettingsManager::Mod->getSettingValue<bool>("show-featured-rank"),
    SettingsManager::Mod->getSettingValue<bool>("show-object-count"),
    SettingsManager::Mod->getSettingValue<bool>("show-ldm-object-count"),
    SettingsManager::Mod->getSettingValue<bool>("show-game-version"),
    SettingsManager::Mod->getSettingValue<bool>("show-level-version"),
    SettingsManager::Mod->getSettingValue<bool>("show-ldm-existence"),
    SettingsManager::Mod->getSettingValue<bool>("show-sent"),
    SettingsManager::Mod->getSettingValue<bool>("show-level-id"),
    SettingsManager::Mod->getSettingValue<bool>("show-original-level-id"),
    SettingsManager::Mod->getSettingValue<bool>("show-two-player-mode"),
    SettingsManager::Mod->getSettingValue<bool>("show-editor-time"),
    SettingsManager::Mod->getSettingValue<bool>("show-editor-time-copies"),
    SettingsManager::Mod->getSettingValue<bool>("show-total-attempts"),
    SettingsManager::Mod->getSettingValue<bool>("show-total-jumps"),
    SettingsManager::Mod->getSettingValue<bool>("show-clicks"),
    SettingsManager::Mod->getSettingValue<bool>("show-attempt-time")
};

// There is DEFINETELY a better way to do this but if it works it works
$execute {
    // Display Settings
    geode::listenForSettingChanges<int64_t>("text-width-offset", [](int64_t offset) {
        SettingsManager::Display.widthOffset = static_cast<int>(offset);
    });
    geode::listenForSettingChanges<int64_t>("text-height-offset", [](int64_t offset) {
        SettingsManager::Display.heightOffset = static_cast<int>(offset);
    });
    geode::listenForSettingChanges<double>("text-size", [](double size) {
        SettingsManager::Display.size = static_cast<float>(size);
    });
    geode::listenForSettingChanges<int64_t>("text-opacity", [](int64_t opacity) {
        SettingsManager::Display.opacity = static_cast<int>(round(opacity / 100.f * 255));
    });
    geode::listenForSettingChanges<cocos2d::ccColor3B>("text-color", [](cocos2d::ccColor3B color) {
        SettingsManager::Display.color = color;
    });
    geode::listenForSettingChanges<std::string>("number-separator", [](std::string separator) {
        SettingsManager::Display.separator = separator;
    });

    // Toggles Settings
    geode::listenForSettingChanges<bool>("show-requested-stars", [](bool enabled) {
        SettingsManager::Toggles.requestedStars = enabled;
    });
    geode::listenForSettingChanges<bool>("show-featured-rank", [](bool enabled) {
        SettingsManager::Toggles.featuredRank = enabled;
    });
    geode::listenForSettingChanges<bool>("show-object-count", [](bool enabled) {
        SettingsManager::Toggles.objectCount = enabled;
    });
    geode::listenForSettingChanges<bool>("show-ldm-object-count", [](bool enabled) {
        SettingsManager::Toggles.ldmObjectCount = enabled;
    });
    geode::listenForSettingChanges<bool>("show-game-version", [](bool enabled) {
        SettingsManager::Toggles.gameVersion = enabled;
    });
    geode::listenForSettingChanges<bool>("show-level-version", [](bool enabled) {
        SettingsManager::Toggles.levelVersion = enabled;
    });
    geode::listenForSettingChanges<bool>("show-ldm-existence", [](bool enabled) {
        SettingsManager::Toggles.ldmExistence = enabled;
    });
    geode::listenForSettingChanges<bool>("show-sent", [](bool enabled) {
        if (!enabled)
            SentCacheManager::Clear();

        SettingsManager::Toggles.sent = enabled;
    });
    geode::listenForSettingChanges<bool>("show-level-id", [](bool enabled) {
        SettingsManager::Toggles.levelID = enabled;
    });
    geode::listenForSettingChanges<bool>("show-original-level-id", [](bool enabled) {
        SettingsManager::Toggles.originalLevel = enabled;
    });
    geode::listenForSettingChanges<bool>("show-two-player-mode", [](bool enabled) {
        SettingsManager::Toggles.twoPlayerMode = enabled;
    });
    geode::listenForSettingChanges<bool>("show-editor-time", [](bool enabled) {
        SettingsManager::Toggles.editorTime = enabled;
    });
    geode::listenForSettingChanges<bool>("show-editor-time-copies", [](bool enabled) {
        SettingsManager::Toggles.editorTimeCopies = enabled;
    });
    geode::listenForSettingChanges<bool>("show-total-attempts", [](bool enabled) {
        SettingsManager::Toggles.totalAttempts = enabled;
    });
    geode::listenForSettingChanges<bool>("show-total-jumps", [](bool enabled) {
        SettingsManager::Toggles.totalJumps = enabled;
    });
    geode::listenForSettingChanges<bool>("show-clicks", [](bool enabled) {
        SettingsManager::Toggles.clicks = enabled;
    });
    geode::listenForSettingChanges<bool>("show-attempt-time", [](bool enabled) {
        SettingsManager::Toggles.attemptTime = enabled;
    });
};