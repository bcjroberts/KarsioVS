#pragma once
#include "C:\Users\Neb2\Desktop\KarsioVS\src\Engine\Component.h"
#include <glm/detail/type_vec3.hpp>
#include <string>

class FloatingTextComponent : public Component {

public:
    FloatingTextComponent(Entity* relativeEnt, glm::vec3 color, float scale, std::string* text);
    void updateTextPosition(float timeDiff);
    ~FloatingTextComponent();
    int myTextId = -1;
    float simCreationTime = 0.f;
    Entity* myRelativeEnt;
};

