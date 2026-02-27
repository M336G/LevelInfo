#include "SentCacheManager.h"

std::unordered_map<int, bool> SentCacheManager::Cache = {};

void SentCacheManager::SaveLevel(int levelID, bool sent) {
    if (SentCacheManager::Cache.size() >= 1000)
        SentCacheManager::Cache.erase(SentCacheManager::Cache.begin());

    SentCacheManager::Cache[levelID] = sent;
};

std::optional<bool> SentCacheManager::GetLevel(int levelID) {
    if (SentCacheManager::Cache.contains(levelID))
        return SentCacheManager::Cache[levelID];
    return std::nullopt;
};

void SentCacheManager::Clear() {
    SentCacheManager::Cache.clear();
};