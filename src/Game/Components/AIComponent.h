#pragma once
#include "../../Engine/Component.h"
#include <vector>
#include <glm/detail/type_vec3.hpp>

class AIComponent : public Component
{
private:
	bool collected = false;

	int minedID = -2;
	int attackerID = -1;
	int personality = rand() % NUM_PERSONALITIES;
public:
	enum personalityTypes { GRINDER, KILL_ONLY, RANDOM, GLASS_CANNON, DEFENSIVE, NUM_PERSONALITIES };

	std::vector<glm::vec3> path;
	Entity * goal = nullptr;
	int state = 0;
	int prevstate = -1;
	glm::vec3 prevPos;
	int notMoving = 0;
	int orbiting = 0;
	
    bool isDestinationInLOS = false; // Is the destination within our line of sight
    float lastLOSCheckTime = 0.f;

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
	void setAttackerID(int id);
	int getAttackerID();
	int getPersonality();
	void setPersonality(int npersonality);
    ~AIComponent();
};

