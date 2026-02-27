#pragma once

class Utils {
private:
    static std::unordered_map<int, std::string> GameVersions;

public:
    static std::string GetGameVersion(int gameVersion);

    static std::string FormatNumber(int number);
    static std::string FormatTime(std::chrono::seconds seconds);
};