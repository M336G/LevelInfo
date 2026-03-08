#include "SettingsManager.h"
#include "SentCacheManager.h"

using namespace geode::prelude;

// First initialize all the parameters
CustomStruct::DisplaySettings SettingsManager::Display = {
    Mod::get()->getSettingValue<int>("text-width-offset"),
    Mod::get()->getSettingValue<int>("text-height-offset"),
    Mod::get()->getSettingValue<float>("text-size"),
    round(Mod::get()->getSettingValue<int>("text-opacity") / 100.f * 255),
    Mod::get()->getSettingValue<ccColor3B>("text-color"),
    Mod::get()->getSettingValue<std::string>("number-separator")
};

CustomStruct::ToggleSettings SettingsManager::Toggles = {
    Mod::get()->getSettingValue<bool>("show-requested-stars"),
    Mod::get()->getSettingValue<bool>("show-featured-rank"),
    Mod::get()->getSettingValue<bool>("show-object-count"),
    Mod::get()->getSettingValue<bool>("show-ldm-object-count"),
    Mod::get()->getSettingValue<bool>("show-game-version"),
    Mod::get()->getSettingValue<bool>("show-level-version"),
    Mod::get()->getSettingValue<bool>("show-ldm-existence"),
    Mod::get()->getSettingValue<bool>("show-sent"),
    Mod::get()->getSettingValue<bool>("show-level-id"),
    Mod::get()->getSettingValue<bool>("show-original-level-id"),
    Mod::get()->getSettingValue<bool>("show-two-player-mode"),
    Mod::get()->getSettingValue<bool>("show-editor-time"),
    Mod::get()->getSettingValue<bool>("show-editor-time-copies"),
    Mod::get()->getSettingValue<bool>("show-attempts"),
    Mod::get()->getSettingValue<bool>("show-jumps"),
    Mod::get()->getSettingValue<bool>("show-clicks"),
    Mod::get()->getSettingValue<bool>("show-attempt-time")
};

CustomStruct::OtherSettings SettingsManager::Other = {
    Mod::get()->getSettingValue<std::string>("senddb-api-url"),
    !Mod::get()->getSettingValue<bool>("disable-gdps-warning"),
};

// There is DEFINETELY a better way to do this but if it works it works
$execute {
    // Display Settings
    listenForSettingChanges<int>("text-width-offset", [](int offset) {
        SettingsManager::Display.widthOffset = offset;
    });
    listenForSettingChanges<int>("text-height-offset", [](int offset) {
        SettingsManager::Display.heightOffset = offset;
    });
    listenForSettingChanges<float>("text-size", [](float size) {
        SettingsManager::Display.size = size;
    });
    listenForSettingChanges<int>("text-opacity", [](int opacity) {
        SettingsManager::Display.opacity = round(opacity / 100.f * 255);
    });
    listenForSettingChanges<ccColor3B>("text-color", [](ccColor3B color) {
        SettingsManager::Display.color = color;
    });
    listenForSettingChanges<std::string>("number-separator", [](std::string_view separator) {
        SettingsManager::Display.separator = separator;
    });

    // Toggles Settings
    listenForSettingChanges<bool>("show-requested-stars", [](bool enabled) {
        SettingsManager::Toggles.requestedStars = enabled;
    });
    listenForSettingChanges<bool>("show-featured-rank", [](bool enabled) {
        SettingsManager::Toggles.featuredRank = enabled;
    });
    listenForSettingChanges<bool>("show-object-count", [](bool enabled) {
        SettingsManager::Toggles.objectCount = enabled;
    });
    listenForSettingChanges<bool>("show-ldm-object-count", [](bool enabled) {
        SettingsManager::Toggles.ldmObjectCount = enabled;
    });
    listenForSettingChanges<bool>("show-game-version", [](bool enabled) {
        SettingsManager::Toggles.gameVersion = enabled;
    });
    listenForSettingChanges<bool>("show-level-version", [](bool enabled) {
        SettingsManager::Toggles.levelVersion = enabled;
    });
    listenForSettingChanges<bool>("show-ldm-existence", [](bool enabled) {
        SettingsManager::Toggles.ldmExistence = enabled;
    });
    listenForSettingChanges<bool>("show-sent", [](bool enabled) {
        if (!enabled)
            SentCacheManager::Clear();

        SettingsManager::Toggles.sent = enabled;
    });
    listenForSettingChanges<bool>("show-level-id", [](bool enabled) {
        SettingsManager::Toggles.levelID = enabled;
    });
    listenForSettingChanges<bool>("show-original-level-id", [](bool enabled) {
        SettingsManager::Toggles.originalLevel = enabled;
    });
    listenForSettingChanges<bool>("show-two-player-mode", [](bool enabled) {
        SettingsManager::Toggles.twoPlayerMode = enabled;
    });
    listenForSettingChanges<bool>("show-editor-time", [](bool enabled) {
        SettingsManager::Toggles.editorTime = enabled;
    });
    listenForSettingChanges<bool>("show-editor-time-copies", [](bool enabled) {
        SettingsManager::Toggles.editorTimeCopies = enabled;
    });
    listenForSettingChanges<bool>("show-attempts", [](bool enabled) {
        SettingsManager::Toggles.attempts = enabled;
    });
    listenForSettingChanges<bool>("show-jumps", [](bool enabled) {
        SettingsManager::Toggles.jumps = enabled;
    });
    listenForSettingChanges<bool>("show-clicks", [](bool enabled) {
        SettingsManager::Toggles.clicks = enabled;
    });
    listenForSettingChanges<bool>("show-attempt-time", [](bool enabled) {
        SettingsManager::Toggles.attemptTime = enabled;
    });

    // Other Settings
    listenForSettingChanges<std::string>("senddb-api-url", [](std::string url) {
        SettingsManager::Other.sendDbApiUrl = url;
    });
    listenForSettingChanges<bool>("disable-gdps-warning", [](bool enabled) {
        SettingsManager::Other.showGDPSWarning = !enabled;
    });
};

bool SettingsManager::ShowedGDPSWarning = false;