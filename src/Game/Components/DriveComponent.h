#pragma once
#include "../../Engine/Component.h"
#include <vehicle/PxVehicleUtilControl.h>

// Drive component. Ideally, all vehicles should have this. 
// Is controlled by a separate driver component, either an AI Driver or a player Driver

// Temp struct
struct vehicleInput {
    bool accel;
    bool brake;
    bool handbrake;
    bool steerLeft;
    bool steerRight;
    bool gearUp;
    bool gearDown;
    bool inReverse;
};

class DriveComponent : public Component
{
private:
    physx::PxVehicleDrive4WRawInputData* myInput;
public:
    bool accel;
    bool brake;
    bool handbrake;
    bool steerLeft;     
    bool steerRight;    
    bool gearUp;
    bool gearDown;
    bool inReverse;

    void setInputs(const bool inAccel, const bool inBrake, const bool inHandbrake, const bool inSteerLeft, const bool inSteerRight, const bool inGearUp, const bool inGearDown);
    void setInputs(vehicleInput);

    bool getAccel(){return accel;};
    bool getBrake(){return brake;};
    bool getHandbrake(){return handbrake;};
    bool getSteerLeft(){return steerLeft;};
    bool getSteerRight() { return steerRight; };
    bool getGearUp() { return gearUp; };
    bool getGearDown() { return gearDown; };
    bool getInReverse() { return inReverse; };

    DriveComponent(physx::PxVehicleDrive4WRawInputData* newInput);
    ~DriveComponent();
};