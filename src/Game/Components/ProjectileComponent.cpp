#include "ProjectileComponent.h"
#include "../../Engine/Entity.h"
#include <geometry/PxGeometryQuery.h>
#include "../../Engine/PhysicsEngine/PhysicsEngine.h"
#include "HealthComponent.h"
#include "AIComponent.h"
#include "../../Engine/Core.h"

ProjectileComponent::ProjectileComponent(int nownerid, float nspeed, float ndamage) : Component(PROJECTILE) {
    ownerid = nownerid;
    speed = nspeed;
    damage = ndamage;
    distanceToCheck = speed * (1.f/60.f) * 2.f;
    previousSimTime = Core::simtimeSinceStartup - (1.f/60.f);
}


ProjectileComponent::~ProjectileComponent() = default;

bool ProjectileComponent::checkForHit() {

    // Fires a raycast to see if anything was hit
    physx::PxRaycastBuffer hitBuf;

    // If we hit something, make sure it is not our vehicle before returning true
    if (PhysicsEngine::getInstance()->fireRaycast(&hitBuf, PhysicsEngine::toPxVec3(owner->getPosition()), PhysicsEngine::toPxVec3(owner->getForwardVector()), distanceToCheck)) {
        Entity* hitEnt = static_cast<Entity*>(hitBuf.block.actor->userData);
        if (hitEnt != nullptr && hitEnt->id == ownerid) {
            return false;
        }
        // TODO: Apply damage to the thing we hit if it has a health component.
        if (hitEnt != nullptr) {
            HealthComponent* hc = static_cast<HealthComponent*>(hitEnt->getComponent(HEALTH));
			if (hc != nullptr) {
				hc->applyDamage(damage);
				// let AI know who hit it
				if (hitEnt->getComponent(AI) != nullptr) {
					static_cast<AIComponent*>(hitEnt->getComponent(AI))->setAttackerID(ownerid);
				}
			}
        }
        return true;
    }

    // If something was hit that is not our own vehicle, returning true will destroy this entity.
    return false;
}

void ProjectileComponent::move() {
    glm::vec3 newPos = owner->getPosition() + (owner->getForwardVector() * speed * (Core::simtimeSinceStartup - previousSimTime));
    owner->updatePosition(newPos);
    previousSimTime = Core::simtimeSinceStartup;
}
