#include "VehiclePhysicsComponent.h"



VehiclePhysicsComponent::VehiclePhysicsComponent(physx::PxVehicleDrive4W* vehicle4w) : PhysicsComponent(vehicle4w->getRigidDynamicActor())
{
    myVehicle = vehicle4w;
}


VehiclePhysicsComponent::~VehiclePhysicsComponent()
{
}
