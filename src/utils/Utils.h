#pragma once

class Utils {
private:
    static std::unordered_map<int, std::string_view> GameVersions;

    static std::string GetBaseURL();

public:
    static std::string_view GetGameVersion(int gameVersion);

    static bool IsGDPS();

    static std::string FormatNumber(int number);
    static std::string FormatTime(std::chrono::seconds seconds);
};