#include <Geode/modify/MenuLayer.hpp>
#include "../managers/SettingsManager.h"
#include "../utils/Utils.h"

using namespace geode::prelude;

class $modify(MenuLayer) {
    bool init() {
        if (!MenuLayer::init())
            return false;

        if (SettingsManager::Toggles.sent &&
            Utils::IsGDPS() &&
            SettingsManager::Other.customSendsEndpoint.empty() &&
            SettingsManager::Other.showGDPSWarning &&
            !SettingsManager::ShowedGDPSWarning
        ) {
            Notification::create(
                "Level Info's sent indicator was disabled (you're on a GDPS)",
                NotificationIcon::Warning,
                1.5f
            )->show();
            Notification::create(
                "Change the sent indicator URL in Level Info's settings",
                NotificationIcon::Warning,
                1.5f
            )->show();

            SettingsManager::ShowedGDPSWarning = true;
        }

        return true;
    };
};