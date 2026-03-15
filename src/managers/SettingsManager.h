#pragma once
#include "../utils/CustomStruct.h"

class SettingsManager {
public:
    static CustomStruct::DisplaySettings Display;
    static CustomStruct::ToggleSettings Toggles;
    static CustomStruct::OtherSettings Other;

    static bool ShowedGDPSWarning;
};