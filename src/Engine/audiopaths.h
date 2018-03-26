#pragma once
#include <string>
class AudioPaths
{
public:
    static std::string bgm1;
    static std::string engineIdle;
    static std::string engineRev;
    static std::string rifleShot;
    
    static std::string pickup1;
    static std::string pickup2;
    static std::string pickup3;

    static std::string metalBump1;
    static std::string metalBump2;

    static std::string crysBump1;
    static std::string crysBump2;
    static std::string crysBreak1;
    static std::string crysBreak2;
    static std::string crysBreak3;

    AudioPaths();
    ~AudioPaths();
};

