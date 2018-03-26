#pragma once
#include "event.h"
#include <string>
#include "../../deps/include/glm/glm.hpp"

class AudioEvent : public Event
{
public:
    glm::vec3 position;
    std::string soundfile;
    AudioEvent(glm::vec3 position, std::string newSound);
    ~AudioEvent();
};

