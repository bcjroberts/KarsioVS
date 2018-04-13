#include "WeaponComponent.h"
#include "../../Engine/Core.h"
#include "../../Engine/EntityManager.h"
#include "../../Engine/Entity.h"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/vector_angle.inl>
#include "UpgradeComponent.h"

WeaponComponent::WeaponComponent(ShapeRendererComponent* newWeapon) : Component(WEAPON) {
    setROF(100.0f);
    lastTimeFired = Core::simtimeSinceStartup;
    projectileSpeed = 2.0f;
    gunDamage = 5.0f;
    myWeapon = newWeapon;
}


WeaponComponent::~WeaponComponent() = default;

float projHeight[3] = {2.7f, 3.7f, 5.4f};

void WeaponComponent::fireWeapon() {
    // First check to see if we can fire
    if (Core::simtimeSinceStartup - lastTimeFired > timeBetweenShots) {
        // If we can fire, update the time value
        lastTimeFired = Core::simtimeSinceStartup;
        // Now spawn the projectile with the correct weapon stats
		float xvalue = float((rand() % 1000) - 500) / 30000.f;
		float yvalue = float((rand() % 1000) - 500) / 30000.f;
		float zvalue = float((rand() % 1000) - 500) / 30000.f;
        int lvl = static_cast<UpgradeComponent*>(owner->getComponent(UPGRADE))->getChassisLevel();

		glm::quat projRot = glm::quat(glm::eulerAngles(glm::quat(myWeapon->getMatrixNoScale())) + glm::vec3(xvalue, yvalue, zvalue));
        EntityManager::getInstance()->createProjectile(owner->id, owner->getPosition() + glm::vec3(0,projHeight[lvl - 1],0), projRot, projectileSpeed, gunDamage);
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

float aimHeight[3] = {2.8f, 3.8f, 7.f};
void WeaponComponent::updateTargetting() {
    // Rotate the gun as much as possible to point at the entity
    if (targetEnt != nullptr) {

        // Aim higher the higher the level of the enemy
        int targetLevel = static_cast<UpgradeComponent*>(owner->getComponent(UPGRADE))->getChassisLevel();

        const glm::vec3 targetPos = targetEnt->getPosition() + glm::vec3(0, projHeight[targetLevel-1] - 1.0f, 0);

        // Now we need to perform the same oangle calculations that also occur elsewhere
        // And limit the guns rotation to the specified max values
        glm::vec3 myYawPos = owner->getPosition();
        myYawPos.y = 0;

        const glm::vec3 yawTargetPos(targetPos.x, 0, targetPos.z);
        float yawAngleToTarget = -glm::orientedAngle(glm::normalize(yawTargetPos - myYawPos), owner->getForwardVector(), owner->getUpVector());

        float pitchAngleToTarget = 0;
        if (abs(yawAngleToTarget) < maxGunYaw) {
            int lvl = static_cast<UpgradeComponent*>(owner->getComponent(UPGRADE))->getChassisLevel();
            pitchAngleToTarget = -glm::orientedAngle(glm::normalize(targetPos - (owner->getPosition() + glm::vec3(0,aimHeight[lvl - 1],0))), glm::normalize(targetPos - owner->getPosition()), owner->getRightVector());

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
