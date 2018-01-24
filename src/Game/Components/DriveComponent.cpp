#include "DriveComponent.h"

DriveComponent::DriveComponent() : Component(DRIVE)
{
};

void DriveComponent::StartAccel() {
    accelerate = true;
};

void DriveComponent::StopAccel() {
    accelerate = false;
}

void DriveComponent::StartBrake() {
    brake = true;
}

void DriveComponent::StopBrake() {
    brake = false;
}


DriveComponent::~DriveComponent() = default;