#pragma once

namespace CustomStruct {
    struct DisplaySettings {
        int widthOffset;
        int heightOffset;
        float size;
        int opacity;
        cocos2d::ccColor3B color;
        std::string separator;

        cocos2d::CCPoint getPosition() const {
            auto winSize = cocos2d::CCDirector::get()->getWinSize();

            return {
                winSize.width * 0.2f + widthOffset,
                winSize.height * 0.7f + heightOffset
            };
        }
    };

    struct ToggleSettings {
        bool requestedStars;
        bool featuredRank;
        bool objectCount;
        bool ldmObjectCount;
        bool gameVersion;
        bool levelVersion;
        bool ldmExistence;
        bool sent;
        bool levelID;
        bool originalLevel;
        bool twoPlayerMode;
        bool editorTime;
        bool editorTimeCopies;
        bool attempts;
        bool jumps;
        bool clicks;
        bool attemptTime;

        bool anyEnabled() const {
            return requestedStars || featuredRank || objectCount || ldmObjectCount || gameVersion ||
                levelVersion || ldmExistence || sent || levelID || originalLevel || twoPlayerMode ||
                editorTime || editorTimeCopies || attempts || jumps || clicks || attemptTime;
        }
    };
}