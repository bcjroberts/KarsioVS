#include "WeaponComponent.h"
#include "../../Engine/Core.h"
#include "../../Engine/EntityManager.h"
#include "../../Engine/Entity.h"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/vector_angle.inl>

WeaponComponent::WeaponComponent(ShapeRendererComponent* newWeapon) : Component(WEAPON) {
    setROF(100.0f);
    lastTimeFired = Core::simtimeSinceStartup;
    projectileSpeed = 2.0f;
    gunDamage = 5.0f;
    myWeapon = newWeapon;
}


WeaponComponent::~WeaponComponent() = default;

void WeaponComponent::fireWeapon() {
    // First check to see if we can fire
    if (Core::simtimeSinceStartup - lastTimeFired > timeBetweenShots) {
        // If we can fire, update the time value
        lastTimeFired = Core::simtimeSinceStartup;
        // Now spawn the projectile with the correct weapon stats
		float xvalue = float((rand() % 1000) - 500) / 30000.f;
		float yvalue = float((rand() % 1000) - 500) / 30000.f;
		float zvalue = float((rand() % 1000) - 500) / 30000.f;

		glm::quat projRot = glm::quat(glm::eulerAngles(glm::quat(myWeapon->getMatrixNoScale())) + glm::vec3(xvalue, yvalue, zvalue));
        EntityManager::getInstance()->createProjectile(owner->id, owner->getPosition() + glm::vec3(0,myWeapon->localPos.y * 0.8f,0), projRot, projectileSpeed, gunDamage);
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

void WeaponComponent::updateTargetting() {
    // Rotate the gun as much as possible to point at the entity
    if (targetEnt != nullptr) {
        glm::vec3 targetPos = targetEnt->getPosition();

        // Now we need to perform the same oangle calculations that also occur elsewhere
        // And limit the guns rotation to the specified max values
        glm::vec3 myPos = owner->getPosition();
        myPos.y = 0;
        targetPos.y = 0;

        float yawAngleToTarget = -glm::orientedAngle(glm::normalize(targetPos - myPos), owner->getForwardVector(), owner->getUpVector());

        float pitchAngleToTarget = 0;
        if (abs(yawAngleToTarget) < maxGunYaw) {
            pitchAngleToTarget = -glm::orientedAngle(glm::normalize((targetPos + glm::vec3(0, 2.f, 0)) - (owner->getPosition() + glm::vec3(0,myWeapon->localPos.y * 0.8f,0))), glm::normalize(targetPos - myPos), owner->getRightVector());

            pitchAngleToTarget = pitchAngleToTarget > maxGunPitch ? maxGunPitch : pitchAngleToTarget;
            pitchAngleToTarget = pitchAngleToTarget <  -maxGunPitch ? 6.283185f - maxGunPitch : 6.283185f + pitchAngleToTarget;
        }
        
        yawAngleToTarget = yawAngleToTarget > maxGunYaw ? maxGunYaw : yawAngleToTarget;
        if (yawAngleToTarget < 0) {
            yawAngleToTarget = yawAngleToTarget < -maxGunYaw ? 6.283185f - maxGunYaw : 6.283185f + yawAngleToTarget;
        }

        myWeapon->rotation = glm::quat(glm::vec3(pitchAngleToTarget, yawAngleToTarget, 0));

    } else { // If no target, set the gun rotation to nothing
        myWeapon->rotation = glm::quat(glm::vec3(0));
    }
}

void WeaponComponent::updateTarget(Entity* newTarget) {
    targetEnt = newTarget;
}
