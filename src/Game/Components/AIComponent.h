#pragma once
#include "../../Engine/Component.h"
#include <vector>
#include <glm/detail/type_vec3.hpp>

class AIComponent : public Component
{
private:
	bool collected = false;
	int minedID = -2;
public:
	std::vector<glm::vec3> path;
	Entity * goal;
	int state = 0;
	int prevstate = -1;
	glm::vec3 prevPos;
	int notMoving = 0;
	int orbiting = 0;
	int goalID = -1;
	
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
	
    ~AIComponent();
};

