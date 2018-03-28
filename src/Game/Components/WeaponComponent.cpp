#include "WeaponComponent.h"
#include "../../Engine/Core.h"
#include "../../Engine/EntityManager.h"
#include "../../Engine/Entity.h"
#include <glm/gtc/quaternion.hpp>

WeaponComponent::WeaponComponent() : Component(WEAPON) {
    setROF(1.0f);
    lastTimeFired = Core::simtimeSinceStartup;
    projectileSpeed = 2.0f;
    gunDamage = 5.0f;
}


WeaponComponent::~WeaponComponent() = default;

void WeaponComponent::fireWeapon() {
    // First check to see if we can fire
    if (Core::simtimeSinceStartup - lastTimeFired > timeBetweenShots) {
        // If we can fire, update the time value
        lastTimeFired = Core::simtimeSinceStartup;
        // Now spawn the projectile with the correct weapon stats
		float xvalue = float((rand() % 2000) - 1000) / 30000.f;
		float yvalue = float((rand() % 2000) - 1000) / 30000.f;
		float zvalue = float((rand() % 2000) - 1000) / 30000.f;
		glm::quat projRot = glm::quat(glm::eulerAngles(owner->getRotation()) + glm::vec3(xvalue, yvalue, zvalue));
        EntityManager::getInstance()->createProjectile(owner->id, owner->getPosition(), projRot, projectileSpeed, gunDamage);
    }
}

void WeaponComponent::setROF(float ROF) {
    timeBetweenShots = 1.0f / ROF;
    gunROF = ROF;
}

void WeaponComponent::updateGunValues(float nROF, float newDamaga, float newSpeed) {
    setROF(nROF);
    gunDamage = newDamaga;
    projectileSpeed = newSpeed;
}
