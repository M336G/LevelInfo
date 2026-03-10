#include <Geode/modify/CCDirector.hpp>
#include "../managers/SettingsManager.h"
#include "../managers/SentCacheManager.h"

using namespace geode::prelude;

class $modify(CCDirector) {
    void purgeDirector() {
        if (SettingsManager::Other.enableSentCache)
            SentCacheManager::SaveCache();
        else
            SentCacheManager::DeleteCache();

        CCDirector::purgeDirector();
    };
};