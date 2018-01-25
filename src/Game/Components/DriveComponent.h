#pragma once
#include "../../Engine/Component.h"

// Drive component. Ideally, all vehicles should have this. 
// Is controlled by a separate driver component, either an AI Driver or a player Driver
class DriveComponent : public Component
{
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

    DriveComponent();
    ~DriveComponent();
};