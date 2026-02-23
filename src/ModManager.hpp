#pragma once

class ModManager {
public:
    static void addLevelToSentCache(int levelID, bool sent);
    static std::optional<bool> getLevelFromSentCache(int levelID);
private:
    static std::unordered_map<int, bool> SentCache;
};