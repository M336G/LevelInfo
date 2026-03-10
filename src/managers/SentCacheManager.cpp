#include "SentCacheManager.h"
#include "SettingsManager.h"
#include <asp/iter.hpp>

using namespace geode::prelude;

std::unordered_map<int, CustomStruct::SentCacheEntry> SentCacheManager::Cache = {};
std::filesystem::path SentCacheManager::CachePath = Mod::get()->getSaveDir() / "sent_cache.json";

void SentCacheManager::SaveCache() {
    matjson::Value cache = {};

    for (auto& entry : SentCacheManager::Cache)
        cache[std::to_string(entry.first)] = entry.second;

    if (cache.size() == 0)
        return;

    auto result = utils::file::writeToJson(CachePath, cache);
    if (!result.isOk()) {
        log::error("Could not save sent levels cache: {}", result.unwrapErr());
        return;
    }
    
    log::info("Saved {} level(s) from the sent levels cache", cache.size());
};

void SentCacheManager::LoadCache() {
    if (!std::filesystem::exists(SentCacheManager::CachePath))
        return;

    auto result = utils::file::readFromJson<matjson::Value>(SentCacheManager::CachePath);
    if (!result.isOk()) {
        log::error("Could not load sent levels cache: {}", result.unwrapErr());
        return;
    }

    for (auto& [key, value] : result.unwrap())
        SentCacheManager::Cache[utils::numFromString<int>(key).unwrap()] = value.as<CustomStruct::SentCacheEntry>().unwrap();

    log::info("Loaded {} level(s) to the sent levels cache", SentCacheManager::Cache.size());
};

void SentCacheManager::DeleteCache() {
    if (std::filesystem::exists(SentCacheManager::CachePath))
        std::filesystem::remove(SentCacheManager::CachePath);
};

void SentCacheManager::ClearCache(int newLimit) {
    if (newLimit > 0 && SentCacheManager::Cache.size() > newLimit) {
        SentCacheManager::Cache.erase(
            SentCacheManager::Cache.begin(),
            std::next(SentCacheManager::Cache.begin(), SentCacheManager::Cache.size() - newLimit)
        );
    } else {
        SentCacheManager::Cache.clear();
    }
};

void SentCacheManager::SaveLevel(int levelID, bool sent) {
    if (SentCacheManager::Cache.size() >= SettingsManager::Other.maxSentCacheLimit)
        SentCacheManager::Cache.erase(SentCacheManager::Cache.begin());

    SentCacheManager::Cache[levelID] = {
        sent,
        sent ? 0 : std::time(nullptr) // Timestamp will always be 0 for sent levels as to not waste memory (there's no need to check their expiration later on)
    };
};

std::optional<bool> SentCacheManager::GetLevel(int levelID) {
    if (SentCacheManager::Cache.contains(levelID)) {
        auto& entry = SentCacheManager::Cache[levelID];

        if (entry.sent)
            return true;

        // If the level isn't sent, check if it's past the expiration to check if it
        // was sent again
        if (entry.timestamp + SettingsManager::Other.maxSentCacheExpiration > std::time(nullptr))
            return false;

        // If it's past the expiration delete it from the cache as said before
        SentCacheManager::Cache.erase(levelID);
    }

    // Returns nothing if the level isn't in the cache or the level was not sent and
    // was past the expiration
    return std::nullopt;
};

void SentCacheManager::DeleteLevel(int levelID) {
    if (SentCacheManager::Cache.contains(levelID))
        SentCacheManager::Cache.erase(levelID);
};

$execute {
    SentCacheManager::LoadCache();
};