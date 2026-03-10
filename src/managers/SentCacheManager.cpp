#include "SentCacheManager.h"

std::unordered_map<int, CustomStruct::SentCacheEntry> SentCacheManager::Cache = {};

void SentCacheManager::SaveLevel(int levelID, bool sent) {
    if (SentCacheManager::Cache.size() >= 1000)
        SentCacheManager::Cache.erase(SentCacheManager::Cache.begin());

    SentCacheManager::Cache[levelID] = {
        sent,
        sent ? 0 : std::time(nullptr) // Timestamp will always be 0 for sent levels as to not waste memory (there's no need to check their expiration later on)
    };
}

std::optional<bool> SentCacheManager::GetLevel(int levelID) {
    if (SentCacheManager::Cache.contains(levelID)) {
        auto& entry = SentCacheManager::Cache[levelID];

        if (entry.sent)
            return true;

        // If the level isn't sent, check if it's past the expiration to check if it
        // was sent again
        if (entry.timestamp + 5 * 60 > std::time(nullptr))
            return false;

        // If it's past the expiration delete it from the cache as said before
        SentCacheManager::Cache.erase(levelID);
    }

    // Returns nothing if the level isn't in the cache or the level was not sent and
    // was past the expiration
    return std::nullopt;
}

void SentCacheManager::Clear() {
    SentCacheManager::Cache.clear();
};