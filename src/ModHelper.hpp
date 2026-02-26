#pragma once

class ModHelper {
public:
    // Taken from https://github.com/Cvolton/betterinfo-geode/blob/de98c4f6fcbec45d001386103cb4e735cd2ca965/src/utils/ServerUtils.cpp#L21C1-L60C2
    inline static std::string getBaseURL() {
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

        std::string ret = (char*)(geode::base::get() + offset);
        return ret.size() > 34 ? ret.substr(0, 34) : ret;
    };
};