#pragma once

namespace CustomStruct {
    struct DisplaySettings {
        int widthOffset;
        int heightOffset;
        float size;
        int opacity;
        cocos2d::ccColor3B color;
        std::string separator;

        float getWidth() const {
            return cocos2d::CCDirector::get()->getWinSize().width * 0.2 + widthOffset;
        };
        float getHeight() const {
            return cocos2d::CCDirector::get()->getWinSize().height * 0.7 + heightOffset;
        };
    };

    struct ToggleSettings {
        bool requestedStars;
        bool featuredRank;
        bool objectCount;
        bool gameVersion;
        bool levelVersion;
        bool ldmExistence;
        bool sent;
        bool levelID;
        bool originalLevel;
        bool twoPlayerMode;
        bool editorTime;
        bool editorTimeCopies;
        bool totalAttempts;
        bool totalJumps;
        bool clicks;
        bool attemptTime;

        bool anyEnabled() const {
            return requestedStars || featuredRank || objectCount || gameVersion ||
                levelVersion || ldmExistence || sent || levelID || originalLevel ||
                twoPlayerMode || editorTime || editorTimeCopies || totalAttempts || totalJumps ||
                clicks || attemptTime;
        }
    };
}