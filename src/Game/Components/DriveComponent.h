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
};

class DriveComponent : public Component
{
private:
    physx::PxVehicleDrive4WRawInputData* myInput;
    float accel;
    float brake;
    float handbrake;
    float steering;
public:
    void setInputs(const float accel, const float brake, const float handbrake, const float steering);
    void setInputs(vehicleInput);

    float getAccel(){return accel;};
    float getBrake(){return brake;};
    float getHandbrake(){return handbrake;};
    float getSteering(){return steering;};

    DriveComponent(physx::PxVehicleDrive4WRawInputData* newInput);
    ~DriveComponent();
};