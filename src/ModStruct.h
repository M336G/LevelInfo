#pragma once

class ModStruct {
public:
    struct InfoSettings {
        float width;
        float height;
        float size;
        int opacity;
        cocos2d::ccColor3B color;
    };

    struct InfoToggles {
        bool requestedStars;
        bool featuredRank;
        bool objectCount;
        bool gameVersion;
        bool levelVersion;
        bool ldmExistence;
        bool sent;
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
            levelVersion || ldmExistence || sent || originalLevel || twoPlayerMode ||
            editorTime || editorTimeCopies || totalAttempts || totalJumps || clicks ||
            attemptTime;
        }
    };
};