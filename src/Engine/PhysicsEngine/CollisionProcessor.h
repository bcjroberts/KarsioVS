#pragma once

#include <PxSimulationEventCallback.h>
#include <PxContactModifyCallback.h>
#include <vector>
#include "../Entity.h"
#include "../AudioObservable.h"
#include "../audiopaths.h"

class CollisionProcessor : public physx::PxSimulationEventCallback, public physx::PxContactModifyCallback
{
public:
    AudioObservable* audioEvents;
    std::vector<Entity*> destroyedEntities;
	CollisionProcessor();
	~CollisionProcessor();
    void bindAudioObservable(AudioObservable* eventList);
	void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs) override;
	void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count) override;
	void onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count) override;
	void onWake(physx::PxActor** actors, physx::PxU32 count) override;
	void onSleep(physx::PxActor** actors, physx::PxU32 count) override;
	void onAdvance(const physx::PxRigidBody* const* bodyBuffer, const physx::PxTransform* poseBuffer, const physx::PxU32 count) override;
    void onContactModify(physx::PxContactModifyPair* const pairs, physx::PxU32 count) override;
};

