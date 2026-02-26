#include "ModManager.hpp"

void ModManager::addLevelToSentCache(int levelID, bool sent) {
    if (SentCache.size() >= 1000)
        SentCache.erase(SentCache.begin());

    SentCache[levelID] = sent;
}

std::optional<bool> ModManager::getLevelFromSentCache(int levelID) {
    if (SentCache.contains(levelID))
        return SentCache[levelID];
    return std::nullopt;
}

// There is DEFINETELY a better way to do this but if it works it works
$execute {
    // Display Settings
    geode::listenForSettingChanges<int64_t>("text-width-offset", [](int64_t offset) {
        ModManager::DisplaySettings.widthOffset = static_cast<int>(offset);
    });
    geode::listenForSettingChanges<int64_t>("text-height-offset", [](int64_t offset) {
        ModManager::DisplaySettings.heightOffset = static_cast<int>(offset);
    });
    geode::listenForSettingChanges<double>("text-size", [](double size) {
        ModManager::DisplaySettings.size = static_cast<float>(size);
    });
    geode::listenForSettingChanges<int64_t>("text-opacity", [](int64_t opacity) {
        ModManager::DisplaySettings.opacity = static_cast<int>(round(opacity / 100.f * 255));
    });
    geode::listenForSettingChanges<cocos2d::ccColor3B>("text-color", [](cocos2d::ccColor3B color) {
        ModManager::DisplaySettings.color = color;
    });

    // Toggle Settings
    geode::listenForSettingChanges<bool>("show-requested-stars", [](bool enabled) {
        ModManager::ToggleSettings.requestedStars = enabled;
    });
    geode::listenForSettingChanges<bool>("show-featured-rank", [](bool enabled) {
        ModManager::ToggleSettings.featuredRank = enabled;
    });
    geode::listenForSettingChanges<bool>("show-object-count", [](bool enabled) {
        ModManager::ToggleSettings.objectCount = enabled;
    });
    geode::listenForSettingChanges<bool>("show-game-version", [](bool enabled) {
        ModManager::ToggleSettings.gameVersion = enabled;
    });
    geode::listenForSettingChanges<bool>("show-level-version", [](bool enabled) {
        ModManager::ToggleSettings.levelVersion = enabled;
    });
    geode::listenForSettingChanges<bool>("show-ldm-existence", [](bool enabled) {
        ModManager::ToggleSettings.ldmExistence = enabled;
    });
    geode::listenForSettingChanges<bool>("show-sent", [](bool enabled) {
        ModManager::ToggleSettings.sent = enabled;
    });
    geode::listenForSettingChanges<bool>("show-original-level-id", [](bool enabled) {
        ModManager::ToggleSettings.originalLevel = enabled;
    });
    geode::listenForSettingChanges<bool>("show-two-player-mode", [](bool enabled) {
        ModManager::ToggleSettings.twoPlayerMode = enabled;
    });
    geode::listenForSettingChanges<bool>("show-editor-time", [](bool enabled) {
        ModManager::ToggleSettings.editorTime = enabled;
    });
    geode::listenForSettingChanges<bool>("show-editor-time-copies", [](bool enabled) {
        ModManager::ToggleSettings.editorTimeCopies = enabled;
    });
    geode::listenForSettingChanges<bool>("show-total-attempts", [](bool enabled) {
        ModManager::ToggleSettings.totalAttempts = enabled;
    });
    geode::listenForSettingChanges<bool>("show-total-jumps", [](bool enabled) {
        ModManager::ToggleSettings.totalJumps = enabled;
    });
    geode::listenForSettingChanges<bool>("show-clicks", [](bool enabled) {
        ModManager::ToggleSettings.clicks = enabled;
    });
    geode::listenForSettingChanges<bool>("show-attempt-time", [](bool enabled) {
        ModManager::ToggleSettings.attemptTime = enabled;
    });

    // Other Settings
    geode::listenForSettingChanges<bool>("disable-gdps-warning", [](bool enabled) {
        ModManager::OtherSettings.showGDPSWarning = !enabled;
    });
};