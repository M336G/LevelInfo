#include "Utils.h"
#include <km7dev.server_api/include/ServerAPIEvents.hpp>
#include "../managers/SettingsManager.h"

using namespace geode::prelude;

std::unordered_map<int, std::string_view> Utils::GameVersions = {
    { 22, "2.2" },
    { 21, "2.1" },
    { 20, "2.0" },
    { 19, "1.9" },
    { 18, "1.8" },
    { 10, "1.7" }
};

std::string_view Utils::GetGameVersion(int gameVersion) {
    if (Utils::GameVersions.count(gameVersion))
        return Utils::GameVersions.at(gameVersion);
    return "Pre-1.7";
};

// Taken from https://github.com/Cvolton/betterinfo-geode/blob/0110f5e273209b7ef0bff1619b88bc74a69efc6c/src/utils/ServerUtils.cpp#L19-L57
std::string Utils::GetBaseURL() {
    if (Loader::get()->isModLoaded("km7dev.server_api")) {
        auto url = ServerAPIEvents::getCurrentServer().url;
        if (!url.empty() && url != "NONE_REGISTERED") {
            while (url.ends_with("/"))
                url.pop_back();
            return url;
        }
    }

    // The addresses are pointing to "https://www.boomlings.com/database/getGJLevels21.php"
    // in the main game executable

    // Prevent compiling if the game version isn't 2.2081 (since these adresses are
    // specific to the binary they won't work if the version's different)
    static_assert(GEODE_COMP_GD_VERSION == 22081, "Unsupported GD version");

    uintptr_t offset;
    #ifdef GEODE_IS_WINDOWS
        offset = 0x558b70;
    #elif defined(GEODE_IS_ARM_MAC)
        offset = 0x77d709;
    #elif defined(GEODE_IS_INTEL_MAC)
        offset = 0x868df0;
    #elif defined(GEODE_IS_ANDROID64)
        offset = 0xECCF90;
    #elif defined(GEODE_IS_ANDROID32)
        offset = 0x96C0DB;
    #elif defined(GEODE_IS_IOS)
        offset = 0x6b8cc2;
    #else
        static_assert(false, "Unsupported platform");
    #endif

    std::string ret = (char*)(base::get() + offset);
    return ret.size() > 34 ? ret.substr(0, 34) : ret;
};

bool Utils::IsGDPS() {
    return Utils::GetBaseURL() != "https://www.boomlings.com/database";
};

// Apparently std::format needs the formats installed so let's just do this manually
std::string Utils::FormatNumber(int number) {
    if (number == 0)
        return "N/A";

    auto string = std::to_string(number);

    if (SettingsManager::Display.separator != "None")
        for (int i = string.size() - 3; i > 0; i -= 3)
            string.insert(i, SettingsManager::Display.separator == "Comma" ? "," : " ");

    return string;
};

std::string Utils::FormatTime(std::chrono::seconds seconds) {
    if (seconds.count() == 0)
        return "N/A";
    
    std::string string;
    auto hours = std::chrono::duration_cast<std::chrono::hours>(seconds).count();
    if (hours > 0)
        string += std::to_string(hours) + "h";

    auto minutes = std::chrono::duration_cast<std::chrono::minutes>(seconds).count() % 60;
    if (minutes > 0)
        string += std::to_string(minutes) + "m";

    string += std::to_string(seconds.count() % 60) + "s";
    
    return string;
};