#pragma once
#include "../utils/CustomStruct.h"

class SentCacheManager {
private:
    static std::unordered_map<int, CustomStruct::SentCacheEntry> Cache;

public:
    static void SaveLevel(int levelID, bool sent);
    static std::optional<bool> GetLevel(int levelID);
    static void Clear(int newLimit = 0);
};