#pragma once

class Utils {
private:
    static std::string const& RequestUserAgent;
    static std::chrono::seconds const RequestTimeout;

public:
    static arc::Future<geode::Result<bool, void>> CheckIfLevelSent(int levelID);

    static std::string GetGameVersion(int gameVersion);

    static bool IsOnGdps();

    static std::string FormatNumber(size_t number);
    static std::string FormatTime(std::chrono::seconds seconds);

    static bool ObjectHasKeyValue(std::string_view object, std::string_view key, std::string_view value);
    static bool ObjectIsLevelSettings(std::string_view object);
};