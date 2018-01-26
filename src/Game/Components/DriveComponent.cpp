#include "DriveComponent.h"

DriveComponent::DriveComponent() : Component(DRIVE)
{
};

void DriveComponent::setInputs(const bool inAccel, const bool inBrake, const bool inHandbrake, const bool inSteerLeft, const bool inSteerRight, const bool inGearUp, const bool inGearDown) {
    accel = inAccel;
    brake = inBrake;
    handbrake = inHandbrake;
    steerLeft = inSteerLeft;
    steerRight = inSteerRight;
    gearUp = inGearUp;
    gearDown = inGearDown;
}

void DriveComponent::setInputs(vehicleInput in) {
    accel = in.accel;
    brake = in.brake;
    handbrake = in.handbrake;
    steerLeft = in.steerLeft;
    steerRight = in.steerRight;
    gearUp = in.gearUp;
    gearDown = in.gearDown;
}
DriveComponent::~DriveComponent() = default;