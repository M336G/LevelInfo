#include "ModManager.h"

void ModManager::addLevelToSentCache(int levelID, bool sent) {
    if (SentCache.size() >= 1000)
        SentCache.erase(SentCache.begin());

    SentCache[levelID] = sent;
}

std::optional<bool> ModManager::getLevelFromSentCache(int levelID) {
    if (SentCache.contains(levelID))
        return SentCache[levelID];
    return std::nullopt;
}