#pragma once
#include "../utils/CustomStruct.h"

class SettingsManager {
private:
    static geode::Mod *Mod;

public:
    static CustomStruct::DisplaySettings Display;
    static CustomStruct::ToggleSettings Toggles;
};