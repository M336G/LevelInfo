#pragma once

class ModManager {
private:
    struct InfoSettings {
        int widthOffset;
        int heightOffset;
        float size;
        int opacity;
        cocos2d::ccColor3B color;

        float getWidth() const {
            return cocos2d::CCDirector::get()->getWinSize().width * 0.2 + widthOffset;
        };
        float getHeight() const {
            return cocos2d::CCDirector::get()->getWinSize().height * 0.7 + heightOffset;
        };
    };

    struct InfoToggles {
        bool requestedStars;
        bool featuredRank;
        bool objectCount;
        bool gameVersion;
        bool levelVersion;
        bool ldmExistence;
        bool sent;
        bool originalLevel;
        bool twoPlayerMode;
        bool editorTime;
        bool editorTimeCopies;
        bool totalAttempts;
        bool totalJumps;
        bool clicks;
        bool attemptTime;

        bool anyEnabled() const {
            return requestedStars || featuredRank || objectCount || gameVersion ||
            levelVersion || ldmExistence || sent || originalLevel || twoPlayerMode ||
            editorTime || editorTimeCopies || totalAttempts || totalJumps || clicks ||
            attemptTime;
        }
    };

    inline static geode::Mod *Mod = geode::Mod::get();

public:
    inline static InfoSettings Settings = {
        static_cast<int>(Mod->getSettingValue<int64_t>("text-width-offset")),
        static_cast<int>(Mod->getSettingValue<int64_t>("text-height-offset")),
        static_cast<float>(Mod->getSettingValue<double>("text-size")),
        static_cast<int>(round(static_cast<double>(Mod->getSettingValue<int64_t>("text-opacity")) / 100 * 255)),
        Mod->getSettingValue<cocos2d::ccColor3B>("text-color")
    };

    inline static InfoToggles Toggles = {
        Mod->getSettingValue<bool>("show-requested-stars"),
        Mod->getSettingValue<bool>("show-featured-rank"),
        Mod->getSettingValue<bool>("show-object-count"),
        Mod->getSettingValue<bool>("show-game-version"),
        Mod->getSettingValue<bool>("show-level-version"),
        Mod->getSettingValue<bool>("show-ldm-existence"),
        Mod->getSettingValue<bool>("show-sent"),
        Mod->getSettingValue<bool>("show-original-level-id"),
        Mod->getSettingValue<bool>("show-two-player-mode"),
        Mod->getSettingValue<bool>("show-editor-time"),
        Mod->getSettingValue<bool>("show-editor-time-copies"),
        Mod->getSettingValue<bool>("show-total-attempts"),
        Mod->getSettingValue<bool>("show-total-jumps"),
        Mod->getSettingValue<bool>("show-clicks"),
        Mod->getSettingValue<bool>("show-attempt-time")
    };

    static void addLevelToSentCache(int levelID, bool sent);
    static std::optional<bool> getLevelFromSentCache(int levelID);

    inline static const std::unordered_map<int, std::string> GameVersions = {
        {22, "2.2"},
        {21, "2.1"},
        {20, "2.0"},
        {19, "1.9"},
        {18, "1.8"},
        {10, "1.7"}
    };
private:
    inline static std::unordered_map<int, bool> SentCache;
};