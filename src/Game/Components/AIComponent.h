#pragma once
#include "../../Engine/Component.h"
#include <vector>
#include <glm/detail/type_vec3.hpp>

class AIComponent : public Component
{
public:
    std::vector<glm::vec3> waypoints;
    int currentWaypointIndex;
    AIComponent();
    glm::vec3 AIComponent::getCurrentWaypoint();
    void nextWaypoint();
    ~AIComponent();
};

