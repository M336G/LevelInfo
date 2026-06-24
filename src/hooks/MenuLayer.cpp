#include <Geode/modify/MenuLayer.hpp>
#include "../managers/SettingsManager.h"
#include "../utils/Utils.h"

using namespace geode::prelude;

class $modify(MenuLayer) {
    bool init() {
        if (!MenuLayer::init())
            return false;

        if (SettingsManager::Toggles.sent &&
            Utils::IsOnGdps() &&
            SettingsManager::Other.customSendsEndpoint.empty() &&
            SettingsManager::Other.showGDPSWarning &&
            !SettingsManager::ShowedGDPSWarning
        ) {
            Notification::create(
                "Level Info's sent indicator was disabled (you're on a GDPS)",
                NotificationIcon::Warning,
                2.f
            )->show();
            Notification::create(
                "Change the sent indicator URL/disable this warning in its settings",
                NotificationIcon::Warning,
                2.f
            )->show();

            SettingsManager::ShowedGDPSWarning = true;
        }

        return true;
    };
};