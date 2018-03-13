#pragma once
#include "../../Engine/Component.h"
#include <vehicle/PxVehicleUtilControl.h>

// Drive component. Ideally, all vehicles should have this. 
// Is controlled by a separate driver component, either an AI Driver or a player Driver

// Temp struct
struct vehicleInput {
    float accel = 0.f;
    float reverse = 0.f;
    float handbrake = 0.f;
    float steering = 0.f;
    bool flip = false;
    bool shoot = false;
};

class DriveComponent : public Component
{
private:
    vehicleInput* myInput;
    physx::PxVehicleDrive4W* myVehicle;
public:
    // Used to determine if the vehicle is allowed to perform a flip.
    float previousFlipTime = 0.f;

    void setInputs(float accel, float reverse, float handbrake, float steering, bool flip = false, bool shoot = false);
    void setInputs(vehicleInput);

    float getAccel() { return myInput->accel; }
    float getBrake() { return myInput->reverse; }
    float getHandbrake() { return myInput->handbrake; }
    float getSteering() { return myInput->steering; }
    bool getFlip() { return myInput->flip; }
    bool getShooting() { return myInput->shoot; }

    physx::PxVehicleDrive4W* getVehicle() { return myVehicle; }

    DriveComponent(vehicleInput* newInput, physx::PxVehicleDrive4W* myVehicle);
    ~DriveComponent();
};