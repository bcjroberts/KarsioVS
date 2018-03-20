#include "AudioEvent.h"



AudioEvent::AudioEvent(int newID, std::string newSound) : Event (PLAYSOUND)
{
    eID = newID;
    soundfile = newSound;
}


AudioEvent::~AudioEvent()
{
}
