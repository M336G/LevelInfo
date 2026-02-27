#include "Utils.h"
#include "../managers/SettingsManager.h"

std::unordered_map<int, std::string> Utils::GameVersions = {
    { 22, "2.2" },
    { 21, "2.1" },
    { 20, "2.0" },
    { 19, "1.9" },
    { 18, "1.8" },
    { 10, "1.7" }
};

std::string Utils::GetGameVersion(int gameVersion) {
    if (Utils::GameVersions.count(gameVersion))
        return Utils::GameVersions.at(gameVersion);
    return "Pre-1.7";
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
}