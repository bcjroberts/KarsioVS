#include "AudioEvent.h"



AudioEvent::AudioEvent(glm::vec3 newPos, std::string newSound) : Event (PLAYSOUND)
{
    position = newPos;
    soundfile = newSound;
}


AudioEvent::~AudioEvent()
{
}
