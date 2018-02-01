#include "CollisionProcessor.h"
#include <cstdio>


CollisionProcessor::CollisionProcessor() {
}


CollisionProcessor::~CollisionProcessor() {
}

void CollisionProcessor::onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs,
	physx::PxU32 nbPairs) {
	printf("Collision detected!\n");
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
