#include "CollisionProcessor.h"
#include <cstdio>
#include <iostream>
#include <PxRigidActor.h>
#include <PxRigidBody.h>
#include "../../Game/Components/HealthComponent.h"
#include "../../Game/Components/AIComponent.h"
#include "../../Game/Components/UpgradeComponent.h"
#include "../../Game/Components/PhysicsComponent.h"
#include "../../Game/Logic/WorldGenerator.h"

CollisionProcessor::CollisionProcessor() {
}


CollisionProcessor::~CollisionProcessor() {
}

void CollisionProcessor::bindAudioObservable(AudioObservable* anEventList) {
    audioEvents = anEventList;
}

// 11: x, 15: y, 19: z
// Use these 3 to caluclate the magnitude of the pre solver velocity collision
void CollisionProcessor::onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs,
	physx::PxU32 nbPairs) {

    // Prints out the float values at 4 bytes intervals
    
    /*for (int si = 3; si < pairHeader.extraDataStreamSize; si+=4)
    {
        float f;
        char b[] = { pairHeader.extraDataStream[si-3], pairHeader.extraDataStream[si - 2], pairHeader.extraDataStream[si - 1], pairHeader.extraDataStream[si] };
        memcpy(&f, &b, sizeof(f));
        printf("Velocity: %i %f\n", si, f);
    }*/

    // Compute the pre solver velocity. Can be used in later calculations for potentially ramming through crystals.
    physx::PxVec3 presolverVelocity(0,0,0);

    float f1;
    char b[] = { pairHeader.extraDataStream[11 - 3], pairHeader.extraDataStream[11 - 2], pairHeader.extraDataStream[11 - 1], pairHeader.extraDataStream[11] };
    memcpy(&f1, &b, sizeof(f1));
    presolverVelocity.x = f1;
    float f2;
    char c[] = { pairHeader.extraDataStream[15 - 3], pairHeader.extraDataStream[15 - 2], pairHeader.extraDataStream[15 - 1], pairHeader.extraDataStream[15] };
    memcpy(&f2, &c, sizeof(f2));
    presolverVelocity.y = f2;
    float f3;
    char d[] = { pairHeader.extraDataStream[19 - 3], pairHeader.extraDataStream[19 - 2], pairHeader.extraDataStream[19 - 1], pairHeader.extraDataStream[19] };
    memcpy(&f3, &d, sizeof(f3));
    presolverVelocity.z = f3;

    physx::PxRigidBody* temp = static_cast<physx::PxRigidBody*>(pairHeader.actors[0]);
    Entity* ent0 = static_cast<Entity*>(pairHeader.actors[0]->userData);
    Entity* ent1 = static_cast<Entity*>(pairHeader.actors[1]->userData);

    // Calculate the mass involved
    float mass0 = static_cast<PhysicsComponent*>(ent0->getComponent(PHYSICS))->getRigidBody()->getMass() / 1000.f;
    float mass1 = static_cast<PhysicsComponent*>(ent1->getComponent(PHYSICS))->getRigidBody()->getMass() / 1000.f;
    float massMulti = mass0 + mass1;

    // Calculate the damage based on the velocity change
    float damage = (temp->getLinearVelocity() - presolverVelocity).magnitude() * massMulti;

    //printf("Pre-solver velocity: %f\n", presolverVelocity.magnitude());
    const char* name0 = pairs[0].shapes[0]->getName();
    const char* name1 = pairs[0].shapes[1]->getName();

    const char* drillstring = "drill";
    if (name0 != nullptr && std::strcmp(name0, drillstring) == 0) // actor 0 hit with a drill so damage actor 1
    {
        static_cast<HealthComponent*>(ent1->getComponent(HEALTH))->applyDamage(damage / mass1, ent0->id);
    }

    if (name1 != nullptr && std::strcmp(name1, drillstring) == 0) // actor 1 hit with a drill so damage actor 0
    {
        static_cast<HealthComponent*>(ent0->getComponent(HEALTH))->applyDamage(damage / mass0, ent1->id);
    }
}

void CollisionProcessor::onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count)
{
	printf("Trigger thingy happened!\n");
}

void CollisionProcessor::onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count)
{
	PX_UNUSED(constraints);
	PX_UNUSED(count);
}

void CollisionProcessor::onWake(physx::PxActor** actors, physx::PxU32 count)
{
	PX_UNUSED(actors); PX_UNUSED(count);
}

void CollisionProcessor::onSleep(physx::PxActor** actors, physx::PxU32 count)
{
	PX_UNUSED(actors); PX_UNUSED(count);
}

void CollisionProcessor::onAdvance(const physx::PxRigidBody* const* bodyBuffer, const physx::PxTransform* poseBuffer,
	const physx::PxU32 count)
{
}

void CollisionProcessor::onContactModify(physx::PxContactModifyPair* const pairs, physx::PxU32 count)
{
    
	// First we need to figure out if the force is great enough to break the crystal.
	// Need 3 things from the vehicles: Its mass, velocity magnitude, and level of drill (multiplier)
	const physx::PxRigidActor* carActor = nullptr;
    Entity* crystalEntity = nullptr;
	Entity* carEntity = nullptr;

	// is the first pair item a car
	if (pairs[0].actor[0]->getNbShapes() > 5) {
		carActor = pairs[0].actor[0];
        crystalEntity = static_cast<Entity*>(pairs[0].actor[1]->userData);
		carEntity = static_cast<Entity*>(carActor->userData);
	} else { // the actor is the car
		carActor = pairs[0].actor[1];
        crystalEntity = static_cast<Entity*>(pairs[0].actor[0]->userData);
		carEntity = static_cast<Entity*>(carActor->userData);
	}
	const physx::PxRigidBody* carBody = static_cast<const physx::PxRigidBody*>(carActor);

    const float ramMulti = static_cast<UpgradeComponent*>(carEntity->getComponent(UPGRADE))->getCurrentRamMultiplier();

    // Use this value to determine if a crystal can be broken with the current ramming force
    //const float damage = carBody->getMass() * carBody->getLinearVelocity().magnitude() / 250.0f * ramMulti;
    const float damage = 1000.f;

    // Apply the ramming damage to the crystal. If the crystal is destroyed, ignore all contacts with it.
    HealthComponent* crystalHealth = static_cast<HealthComponent*>(crystalEntity->getComponent(HEALTH));
    crystalHealth->applyDamage(damage, carEntity->id);

	// tell car it hit a crystal
	AIComponent* carAI = static_cast<AIComponent*>(carEntity->getComponent(AI));
	if (carEntity->getComponent(AI) != nullptr) {
		carAI->setMinedID(crystalEntity->id);
	}

    if (crystalHealth->isDead())
    {
        // Add the resources of the crystal to the car whichd estroyed it
        const float crystalValue = crystalHealth->getMaxHealth();

        if (std::find(crystalEntity->myTags.begin(), crystalEntity->myTags.end(), "HealthCrystal") != crystalEntity->myTags.end()) {
            // This is a health crystal
            static_cast<HealthComponent*>(static_cast<Entity*>(carActor->userData)->getComponent(HEALTH))->addHealth(crystalValue / 2.f);
        } else {
            static_cast<UpgradeComponent*>(static_cast<Entity*>(carActor->userData)->getComponent(UPGRADE))->addResources(crystalValue);
        }

        
        AudioEvent fireEvent = AudioEvent(carEntity->getPosition(), rand()%2>0 ? AudioPaths::getInstance()->crysBreak1 : AudioPaths::getInstance()->crysBreak2);
        audioEvents->notifyAudioObservers(fireEvent);

        // Mark the crystal for destruction
        destroyedEntities.push_back(crystalEntity);

		// remove from world and pathfinding
		WorldGenerator::getInstance()->removeCrystal(crystalEntity);

        for (int i = 0; i < count; i++)
        {
            for (int j = 0; j < pairs[i].contacts.size(); j++)
            {
                pairs[i].contacts.ignore(j);
            }
        }
    }
}

