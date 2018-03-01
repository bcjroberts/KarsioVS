#pragma once
#include "../../Engine/Component.h"
#include <vector>
#include <glm/detail/type_vec3.hpp>

class AIComponent : public Component
{
private:
	bool collected = false;
	int goalID = -1;
	int minedID = -2;
public:
    std::vector<glm::vec3> waypoints;
    int currentWaypointIndex;
    AIComponent();
    glm::vec3 AIComponent::getCurrentWaypoint();
    void nextWaypoint();
	void setWaypoints(std::vector<glm::vec3> path);
	void clearWaypoints();
	void setMinedID(int id);
	int getMinedID();
	void setKilledCrystal(bool ncollected);
	bool getKilledCrystal();
	void setGoalID(int id);
	int getGoalID();
    ~AIComponent();
};

