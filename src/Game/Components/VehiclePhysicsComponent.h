#pragma once
#include "PhysicsComponent.h"

class VehiclePhysicsComponent: public PhysicsComponent
{
private:
    physx::PxVehicleDrive4W* myVehicle;
public:
    VehiclePhysicsComponent(physx::PxVehicleDrive4W* vehicle4w);
    ~VehiclePhysicsComponent();
};

