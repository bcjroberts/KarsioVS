#pragma once
#include "../../Engine/Component.h"
#include <vehicle/PxVehicleUtilControl.h>

// Drive component. Ideally, all vehicles should have this. 
// Is controlled by a separate driver component, either an AI Driver or a player Driver

// Temp struct
struct vehicleInput {
    float accel;
    float brake;
    float handbrake;
    float steering;
    bool flip;
    bool shoot;
};

class DriveComponent : public Component
{
private:
    physx::PxVehicleDrive4WRawInputData* myInput;
    physx::PxVehicleDrive4W* myVehicle;
    float accel;
    float brake;
    float handbrake;
    float steering;
    bool flip;
    bool shoot;
public:
    // Used to determine if the vehicle is allowed to perform a flip.
    float previousFlipTime = 0.f;

    void setInputs(const float accel, const float brake, const float handbrake, const float steering, const bool flip = false, const bool shoot = false);
    void setInputs(vehicleInput);

    float getAccel(){return accel;};
    float getBrake(){return brake;};
    float getHandbrake(){return handbrake;};
    float getSteering(){return steering;};
    bool getFlip() { return flip; };
    bool getShooting() {return shoot;}

    physx::PxVehicleDrive4W* getVehicle() { return myVehicle; }

    DriveComponent(physx::PxVehicleDrive4WRawInputData* newInput, physx::PxVehicleDrive4W* myVehicle);
    ~DriveComponent();
};