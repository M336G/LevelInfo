# Version 1.4.2
- Disable the Sent indicator when on a GDPS (also shows a warning once per session if that's the case, which you can disable in the mod's settings)
- Renamed "Text Settings" to "Display Settings"

# Version 1.4.1
- Optimized settings
    - Settings are now loaded once when the mod loads and automatically update when changed instead of being reloaded every time the player opens LevelInfoLayer

# Version 1.4.0
- Added a loading circle while the level information loads (thanks [cue](https://github.com/dankmeme01/cue)!)
- Changed "Req. Difficulty" to "Requested stars/moons"
- Improved the codebase & tried making it safer/more optimized

# Version 1.3.12
- Added caching for the Sent setting
    - If the level was already loaded during the session with the setting enabled, it won't make another request and will instead get it from the memory (up to 1,000 levels can be cached)

# Version 1.3.11
- Added the Sent setting
- Fixed setting not appearing if all of them were disabled but one was still enabled (only happened with some of them)

# Version 1.3.10
- Added iOS compatibility (still experimental)
- Fixed the mod downloading AutoLeaderboard & Mod Badge Info even though they aren't required

# Version 1.3.9
- Ported the mod to Geode 4.0.0-beta.1 (GD 2.2074)

# Version 1.3.8
- Improved the text's scaling system
- Enhanced mod's settings
- Added colors to the mod's description

# Version 1.3.7
- Fixed stats being incorrect if the level took too long to load or was not downloaded yet
- Code improvements/rewrite
- Re-enabled macOS building

# Version 1.3.6
- Added the LDM Existence setting
- Fixed tags not showing up
- Updated Geode version to v3.0.0

# Version 1.3.5
- Ported the mod to Geode 3.0.0-beta.1
- Temporarily removed macOS compatibility

# Version 1.3.4
- Added the original level ID setting
- Reduced "Edit. Time" and "Edit. Time (+cop.)" to "Editor" and "Edit. (+cop.)"
- Added the "customization" tag
- Improved settings' description
- Code improvements

# Version 1.3.3
- Added the text size setting
- Code improvements & optimization (Thanks wnt\nr!)
- Faster object count calculation on Windows and Android
- Added the "_spr" operator at the end of the label string ID

# Version 1.3.2
- Added text opacity setting
- Lots of code improvements & optimization
- Make beta 23 the minimum version

# Version 1.3.1
- Code optimization
- Added a string ID to the label
- Improved the mod's description in the README.md, the about.md and the mod.json files

# Version 1.3.0
- Added the editor time setting
- Added the editor time (+copies) setting
- Added the total attempt setting
- Added the total jumps setting
- Added the clicks setting
- Added the best time setting
- Fixed the object count setting being locked at 65535 objects (might need to refresh the level if it was not already in the local cache)
- Removed try/catch blocks to avoid crashes on Android
- Changed logs debug to info
- Made logs show the level and its author
- Removed a duplicate main.cpp (which fixed macOS build failing, which means macOS support is back!)
- Fixed the mod getting information about the level and displaying them even if every setting were disabled

# Version 1.2.1
- Optimized the code for many things: Feature state, Object count and Game version (Thanks VolcaroCham!)
- Show levels made before 1.7 as "Pre-1.7"

# Version 1.2.0
- Added the game version settings/stats
- Added GitHub issues
- Optimized the code
- Added logs
- Added error handling
- Added more notes to make the code more understandable
- Made object count display “Unknown” if for some reason the object count was at 0
- Added changelog.md
- New logo by freadfries!

# Version 1.0.1
- Added the utility and interface tags
- Added an option to change the text color
