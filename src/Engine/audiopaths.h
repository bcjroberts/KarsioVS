#pragma once
#include <string>
class AudioPaths
{
private:
    static AudioPaths* globalInstance;
public:
    std::string bgm1;
    std::string engineIdle;
    std::string engineRev;
    std::string rifleShot;
    
    std::string pickup1;
    std::string pickup2;
    std::string pickup3;

    std::string metalBump1;
    std::string metalBump2;

    std::string crysBump1;
    std::string crysBump2;
    std::string crysBreak1;
    std::string crysBreak2;
    std::string crysBreak3;
    
    static AudioPaths* getInstance();

    AudioPaths();
    ~AudioPaths();
};

