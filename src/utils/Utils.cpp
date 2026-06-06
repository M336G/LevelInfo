#include "Utils.h"
#include "../managers/SettingsManager.h"

using namespace geode::prelude;

std::string const& Utils::GetUserAgent() {
    static const auto userAgent = "LevelInfo/" + Mod::get()->getVersion().toNonVString();
    return userAgent;
};

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

// Apparently std::format needs the formats installed so let's just do this manually
std::string Utils::FormatNumber(size_t number) {
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

bool Utils::ObjectHasKeyValue(std::string_view object, std::string_view key, std::string_view value) {
    std::string needle = std::string(key) + "," + std::string(value);

    return object == needle
        || object.starts_with(needle + ",")
        || object.find("," + needle + ",") != std::string::npos
        || object.ends_with("," + needle);
};

// This could have been made more simply but I wanted to be extra sure it doesn't accidentally
// catch something in a Base64 string
bool Utils::ObjectIsLevelSettings(std::string_view object) {
    if (object.starts_with("k"))
        return true;

    bool isKey = true;
    for (auto token : asp::iter::split(object, ',')) {
        if (isKey) {
            for (char character : token) {
                if (character < '0' || character > '9')
                    return true;
            }
        }

        isKey = !isKey;
    }

    return false;
};