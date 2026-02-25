#pragma once

class ModManager {
public:
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