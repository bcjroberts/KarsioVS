#pragma once
#include "../../Engine/Component.h"

// Drive component. Ideally, all vehicles should have this. 
// Is controlled by a separate driver component, either an AI Driver or a player Driver
class DriveComponent : public Component
{
public:
    bool accelerate;
    bool brake;
    bool steerLeft;     // These two are placeholders.
    bool steerRight;    // Currently, I have no clue what datatype PhysX uses to determine "how much" a vehicle turns.

    DriveComponent();
    void StartAccel();
    void StopAccel();
    void StartBrake();
    void StopBrake();
    ~DriveComponent();
};