#include "CollisionProcessor.h"
#include <cstdio>
#include <iostream>
#include <PxRigidActor.h>
#include <PxRigidBody.h>
#include <extensions/PxRigidActorExt.h>


CollisionProcessor::CollisionProcessor() {
}


CollisionProcessor::~CollisionProcessor() {
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
    physx::PxVec3 vvv(0,0,0);

    float f1;
    char b[] = { pairHeader.extraDataStream[11 - 3], pairHeader.extraDataStream[11 - 2], pairHeader.extraDataStream[11 - 1], pairHeader.extraDataStream[11] };
    memcpy(&f1, &b, sizeof(f1));
    vvv.x = f1;
    float f2;
    char c[] = { pairHeader.extraDataStream[15 - 3], pairHeader.extraDataStream[15 - 2], pairHeader.extraDataStream[15 - 1], pairHeader.extraDataStream[15] };
    memcpy(&f2, &c, sizeof(f2));
    vvv.y = f2;
    float f3;
    char d[] = { pairHeader.extraDataStream[19 - 3], pairHeader.extraDataStream[19 - 2], pairHeader.extraDataStream[19 - 1], pairHeader.extraDataStream[19] };
    memcpy(&f3, &d, sizeof(f3));
    vvv.z = f3;

    printf("Pre-solver velocity: %f\n", vvv.magnitude());

    // Grab the rigidbody and calculate the velocity that way as well.
    // Currently this comes out as the same value, so maybe the above code is unneccissary.
    physx::PxRigidActor* temp = pairHeader.actors[0];
    if (temp->getType() == physx::PxActorType::eRIGID_DYNAMIC)
    {
        physx::PxRigidBody* temp2 = static_cast<physx::PxRigidBody*>(temp);
        printf("0: Actual velocity: %f\n", temp2->getLinearVelocity().magnitude());
    }

    temp = pairHeader.actors[1];
    if (temp->getType() == physx::PxActorType::eRIGID_DYNAMIC)
    {
        physx::PxRigidBody* temp2 = static_cast<physx::PxRigidBody*>(temp);
        printf("1: Actual velocity: %f\n", temp2->getLinearVelocity().magnitude());
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
