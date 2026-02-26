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
    geode::listenForSettingChanges<int64_t>("text-width-offset", [](int64_t offset) {
        ModManager::Settings.widthOffset = static_cast<int>(offset);
    });
    geode::listenForSettingChanges<int64_t>("text-height-offset", [](int64_t offset) {
        ModManager::Settings.heightOffset = static_cast<int>(offset);
    });
    geode::listenForSettingChanges<double>("text-size", [](double size) {
        ModManager::Settings.size = static_cast<float>(size);
    });
    geode::listenForSettingChanges<int64_t>("text-opacity", [](int64_t opacity) {
        ModManager::Settings.opacity = static_cast<int>(round(opacity / 100.f * 255));
    });
    geode::listenForSettingChanges<cocos2d::ccColor3B>("text-color", [](cocos2d::ccColor3B color) {
        ModManager::Settings.color = color;
    });

    geode::listenForSettingChanges<bool>("show-requested-stars", [](bool enabled) {
        ModManager::Toggles.requestedStars = enabled;
    });
    geode::listenForSettingChanges<bool>("show-featured-rank", [](bool enabled) {
        ModManager::Toggles.featuredRank = enabled;
    });
    geode::listenForSettingChanges<bool>("show-object-count", [](bool enabled) {
        ModManager::Toggles.objectCount = enabled;
    });
    geode::listenForSettingChanges<bool>("show-game-version", [](bool enabled) {
        ModManager::Toggles.gameVersion = enabled;
    });
    geode::listenForSettingChanges<bool>("show-level-version", [](bool enabled) {
        ModManager::Toggles.levelVersion = enabled;
    });
    geode::listenForSettingChanges<bool>("show-ldm-existence", [](bool enabled) {
        ModManager::Toggles.ldmExistence = enabled;
    });
    geode::listenForSettingChanges<bool>("show-sent", [](bool enabled) {
        ModManager::Toggles.sent = enabled;
    });
    geode::listenForSettingChanges<bool>("show-original-level-id", [](bool enabled) {
        ModManager::Toggles.originalLevel = enabled;
    });
    geode::listenForSettingChanges<bool>("show-two-player-mode", [](bool enabled) {
        ModManager::Toggles.twoPlayerMode = enabled;
    });
    geode::listenForSettingChanges<bool>("show-editor-time", [](bool enabled) {
        ModManager::Toggles.editorTime = enabled;
    });
    geode::listenForSettingChanges<bool>("show-editor-time-copies", [](bool enabled) {
        ModManager::Toggles.editorTimeCopies = enabled;
    });
    geode::listenForSettingChanges<bool>("show-total-attempts", [](bool enabled) {
        ModManager::Toggles.totalAttempts = enabled;
    });
    geode::listenForSettingChanges<bool>("show-total-jumps", [](bool enabled) {
        ModManager::Toggles.totalJumps = enabled;
    });
    geode::listenForSettingChanges<bool>("show-clicks", [](bool enabled) {
        ModManager::Toggles.clicks = enabled;
    });
    geode::listenForSettingChanges<bool>("show-attempt-time", [](bool enabled) {
        ModManager::Toggles.attemptTime = enabled;
    });
};