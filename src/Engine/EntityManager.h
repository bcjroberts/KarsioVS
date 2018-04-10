#pragma once

#include "vector"
#include "Event.h"
#include <glm/gtc/quaternion.hpp>
#include "../game/components/RendererComponent.h"
#include "AudioObservable.h"

class Entity;

class EntityManager
{
private:
	std::vector<Entity*> vehicleEntities;
	std::vector<Entity*> entities;
	static EntityManager* globalInstance;
    AudioObservable* audioEvents;
public:
    EntityManager();
    void bindAudioObservable(AudioObservable* newAudioEvents);
	void destroyEntity(int id);
    void destroyAllEntities();
	Entity* createEntity(glm::vec3 position, glm::quat rotation, glm::vec3 scale);
    Entity* createBasicVehicleEntity(glm::vec3 startPos);
    Entity* createAIVehicleEntity(glm::vec3 startPos);
	Entity* createPlayerVehicleEntity(glm::vec3 startPos);
	Entity* createGroundPlane();
	Entity* createBox(glm::vec3 startPos, glm::vec3 scale);
	Entity* createBoulder(glm::vec3 startPos, glm::vec3 scale);
	Entity* createSmallRock(glm::vec3 startPos, glm::vec3 scale);
	Entity* createWallBoulder(glm::vec3 startPos, glm::vec3 scale);
    Entity* createCrystal(glm::vec3 startPos, float resourceAmount = 0);
	Entity* createCrystalProp(glm::vec3 startPos);
    Entity* createProjectile(int ownerid, glm::vec3 origin, glm::quat orientation, float speed, float damage);
    Entity* EntityManager::createFloatingText (Entity* relativeEnt, glm::vec3 color, float scale, std::string* text);
    void updateChassis(Entity* toUpdate, float newScale, int level);
    void updateGun(Entity* toUpdate, int gunLevel);
    void updateRam(Entity* toUpdate, int ramLevel);
    void updateArmor(Entity* toUpdate, int chassisLevel, int armorLevel);
    static EntityManager* getInstance();
	std::vector<Entity*> getEntities();
	std::vector<Entity*> getVehicleEntities();
    Entity* getVehicleEntityWithID(int id);
    ~EntityManager();
};

