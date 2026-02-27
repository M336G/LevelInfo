#pragma once

class SentCacheManager {
private:
    static std::unordered_map<int, bool> Cache;

public:
    static void SaveLevel(int levelID, bool sent);
    static std::optional<bool> GetLevel(int levelID);
    static void Clear();
};