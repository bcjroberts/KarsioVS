#include "AudioPaths.h"
#include "Core.h"

// Initialize the Entity Manager global pointer.
AudioPaths *AudioPaths::globalInstance = nullptr;

AudioPaths::AudioPaths()
{
    bgm1 = Core::dataPath + "sound/bgm1.mp3";
    engineIdle = Core::dataPath + "sound/engine-idle.mp3";
    engineRev = Core::dataPath + "sound/engine-rev.mp3";
    rifleShot = Core::dataPath + "sound/rifle-close.mp3";

    pickup1 = Core::dataPath + "sound/pickup1.mp3";

    metalBump1 = Core::dataPath + "sound/minor-collide-1.mp3";
    metalBump2 = Core::dataPath + "sound/minor-collide-2.mp3";

    crysBump1 = Core::dataPath + "sound/crystal-collide-1.mp3";
    crysBump2 = Core::dataPath + "sound/crystal-collide-2.mp3";

    crysBreak1 = Core::dataPath + "sound/crystal-shatter-minor-1.mp3";
    crysBreak2 = Core::dataPath + "sound/crystal-shatter-major-1.mp3";
    crysBreak3 = Core::dataPath + "sound/crystal-shatter-major-2.mp3";
}

AudioPaths* AudioPaths::getInstance()
{
    if (!globalInstance) {
        globalInstance = new AudioPaths;
    }
    return globalInstance;
}

AudioPaths::~AudioPaths()
{
}
