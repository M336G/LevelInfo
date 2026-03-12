#pragma once
#include "../utils/CustomStruct.h"
#include <shared_mutex>

class SentCacheManager {
private:
    static std::shared_mutex Mutex;
    static std::unordered_map<int, CustomStruct::SentCacheEntry> Cache;

    static std::filesystem::path CachePath;

public:
    static void SaveCache();
    static void LoadCache();
    static void DeleteCache();
    static void ClearCache(int newLimit = 0);

    static void SaveLevel(int levelID, bool sent);
    static std::optional<bool> GetLevel(int levelID);
    static void DeleteLevel(int levelID);
};