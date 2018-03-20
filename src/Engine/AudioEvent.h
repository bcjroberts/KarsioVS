#pragma once
#include "event.h"
#include <string>

class AudioEvent : public Event
{
public:
    int eID;
    std::string soundfile;
    AudioEvent(int newID, std::string newSound);
    ~AudioEvent();
};

