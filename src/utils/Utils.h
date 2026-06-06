#pragma once

class Utils {
private:
    static std::unordered_map<int, std::string_view> GameVersions;

public:
    static std::string const& GetUserAgent();

    static std::string_view GetGameVersion(int gameVersion);

    static std::string FormatNumber(size_t number);
    static std::string FormatTime(std::chrono::seconds seconds);

    static bool ObjectHasKeyValue(std::string_view object, std::string_view key, std::string_view value);
    static bool ObjectIsLevelSettings(std::string_view object);
};