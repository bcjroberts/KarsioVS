#include "AIComponent.h"


AIComponent::AIComponent() : Component(AI)
{
    // Currently hard coded waypoints, 4 corners to travel to.
    currentWaypointIndex = 0;
    //waypoints.emplace_back(glm::vec3(50, 0, 50));
    //waypoints.emplace_back(glm::vec3(-50, 0, 50));
    //waypoints.emplace_back(glm::vec3(-50, 0, -50));
    //waypoints.emplace_back(glm::vec3(50, 0, -50));
}

glm::vec3 AIComponent::getCurrentWaypoint() {
    if (currentWaypointIndex >= waypoints.size()) return glm::vec3(0);
    return waypoints[currentWaypointIndex];
}

void AIComponent::setWaypoints(std::vector<glm::vec3> path) {
	for (int i = path.size() - 1; i > -1; i--) {
		waypoints.emplace_back(path[i]);
	}
}

void AIComponent::nextWaypoint() {
    currentWaypointIndex = (currentWaypointIndex + 1) % waypoints.size();
}

void AIComponent::clearWaypoints() {
	currentWaypointIndex = 0;
	waypoints.clear();
}

void AIComponent::setMinedID(int id) {
	// set id of the crystal it hit (maybe better name needed)
	minedID = id;
}

int AIComponent::getMinedID() {
	return minedID;
}

void AIComponent::setKilledCrystal(bool ncollected) {
	collected = ncollected;
}

bool AIComponent::getKilledCrystal() {
	return collected;
}

AIComponent::~AIComponent()
{
}
