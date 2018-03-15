#pragma once
#include "../../Engine/Component.h"
#include <glm/detail/type_vec3.hpp>

class StaticLightComponent : public Component
{
private:
    int myLightID = -1;
    glm::vec3 pos;
    glm::vec3 color;
public:
    StaticLightComponent(glm::vec3 position, glm::vec3 color);
    ~StaticLightComponent();
};

