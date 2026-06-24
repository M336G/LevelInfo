#include "Utils.h"
#include <km7dev.server_api/include/ServerAPIEvents.hpp>
#include "../managers/SettingsManager.h"

using namespace geode::prelude;

std::string const& Utils::RequestUserAgent = "LevelInfo/" + Mod::get()->getVersion().toNonVString();
std::chrono::seconds const Utils::RequestTimeout = std::chrono::seconds(3);

arc::Future<Result<bool, void>> Utils::CheckIfLevelSent(int levelID) {
    if (!Utils::IsOnGdps() && SettingsManager::Other.customSendsEndpoint.empty()) {
        auto req = co_await utils::web::WebRequest()
            .userAgent(Utils::RequestUserAgent)
            .timeout(Utils::RequestTimeout)
            .get(fmt::format("https://sdbc.m336.dev/level/{}", levelID));

        auto body = req.json().unwrapOrDefault();
        auto error = body["error"].asString().unwrapOrDefault();

        if (req.ok() && body.size() > 0 && error.empty())
            co_return Ok(body["sent"].asBool().unwrap());
        else
            log::warn(
                "Failed requesting to the SendDB cache API: {}. Falling back to the original SendDB API",
                error.size() > 0 ? error : req.errorMessage()
            );
    }

    auto req = co_await utils::web::WebRequest()
        .userAgent(Utils::RequestUserAgent)
        .timeout(Utils::RequestTimeout)
        .get(SettingsManager::Other.customSendsEndpoint.empty()
            ? "https://api.senddb.dev/api/v1/level/" 
            : SettingsManager::Other.customSendsEndpoint
            + std::to_string(levelID));

    auto body = req.json().unwrapOrDefault();

    if (!req.ok() || body.size() <= 0) {
        auto error = req.errorMessage();

        log::error(
            "Failed requesting to the SendDB API: {}",
            error.size() > 0 ? error : req.string().unwrap()
        );
        co_return Err();
    }

    co_return Ok(body["sends"].size() > 0 ? true : false);
};

std::unordered_map<int, std::string_view> Utils::GameVersions = {
    { 22, "2.2" },
    { 21, "2.1" },
    { 20, "2.0" },
    { 19, "1.9" },
    { 18, "1.8" },
    { 10, "1.7" }
};

// https://github.com/hiimjasmine00/jasmine-tools/blob/6b6a3b00536a341791eb0de33e53b63c49baa8df/src/jasmine.cpp#L39-L66
bool Utils::IsOnGdps() {
    static const bool isOnGdps = []() -> bool {
        std::string url;

        if (Loader::get()->isModLoaded("km7dev.server_api")) {
            url = ServerAPIEvents::getCurrentServer().url;
            if (!url.empty() && url!= "NONE_REGISTERED") {
                while (url.ends_with("/")) url.pop_back();
            } else {
                url = "";
            }
        }

        if (url.empty()) {
            static_assert(GEODE_COMP_GD_VERSION == 22081, "Incompatible GD version for GDPS check");
            url = std::string(
                reinterpret_cast<const char *>(base::get() +
                    GEODE_WINDOWS(0x558b70)
                    GEODE_ARM_MAC(0x77d709)
                    GEODE_INTEL_MAC(0x868df0)
                    GEODE_ANDROID64(0xeccf90)
                    GEODE_ANDROID32(0x96c0db)
                    GEODE_IOS(0x6b8cc2)
                ), 34
            );
        }

        return url.find("://www.boomlings.com/database") == std::string::npos;
    }();

    return isOnGdps;
}

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