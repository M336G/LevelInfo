#include <Geode/modify/MenuLayer.hpp>
#include "../ModManager.hpp"
#include "../ModHelper.hpp"

class $modify(MenuLayer) {
    bool init() {
        if (!MenuLayer::init())
            return false;

        if (ModManager::ToggleSettings.sent && ModManager::IsGDPS && ModManager::OtherSettings.showGDPSWarning && !ModManager::ShowedGDPSWarning) {
            geode::Notification::create(
                "LevelInfo's sent indicator was disabled (you're on a GDPS)",
                geode::NotificationIcon::Warning,
                3.f
            )->show();
            ModManager::ShowedGDPSWarning = true;
        }

        return true;
    };
};