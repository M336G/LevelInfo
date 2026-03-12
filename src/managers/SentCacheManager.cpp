#include "SentCacheManager.h"
#include "SettingsManager.h"

using namespace geode::prelude;

std::shared_mutex SentCacheManager::Mutex;
std::unordered_map<int, CustomStruct::SentCacheEntry> SentCacheManager::Cache = {};

std::filesystem::path SentCacheManager::CachePath = Mod::get()->getSaveDir() / "sent_cache.json";

void SentCacheManager::SaveCache() {
    std::shared_lock lock(Mutex);

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
};

void SentCacheManager::LoadCache() {
    std::unique_lock lock(Mutex);

    if (!std::filesystem::exists(SentCacheManager::CachePath))
        return;

    auto result = utils::file::readFromJson<matjson::Value>(SentCacheManager::CachePath);
    if (!result.isOk()) {
        log::error("Could not load sent levels cache: {}", result.unwrapErr());
        return;
    }

    for (auto& [key, value] : result.unwrap())
        SentCacheManager::Cache[utils::numFromString<int>(key).unwrap()] = value.as<CustomStruct::SentCacheEntry>().unwrap();

    log::info("Loaded {} locally saved level(s) in the sent levels cache", SentCacheManager::Cache.size());
};

void SentCacheManager::DeleteCache() {
    if (std::filesystem::exists(SentCacheManager::CachePath))
        std::filesystem::remove(SentCacheManager::CachePath);
};

void SentCacheManager::ClearCache(int newLimit) {
    std::unique_lock lock(Mutex);

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
    {
        std::unique_lock lock(Mutex);

        if (SentCacheManager::Cache.size() >= SettingsManager::Other.maxSentCacheLimit)
            SentCacheManager::Cache.erase(SentCacheManager::Cache.begin());

        SentCacheManager::Cache[levelID] = {
            sent,
            sent ? 0 : std::time(nullptr) // Timestamp will always be 0 for sent levels as to not waste memory (there's no need to check their expiration later on)
        };
    }
    
    // Save all levels in the sent cache
    async::runtime().spawnBlocking<void>([] {
        SentCacheManager::SaveCache();
    });
};

std::optional<bool> SentCacheManager::GetLevel(int levelID) {
    std::unique_lock lock(Mutex);
    
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
    std::unique_lock lock(Mutex);

    if (SentCacheManager::Cache.contains(levelID))
        SentCacheManager::Cache.erase(levelID);
};

// Load the cache on startup
$execute {
    async::runtime().spawnBlocking<void>([] {
        SentCacheManager::LoadCache();
    });
};