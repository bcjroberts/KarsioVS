#include "DriveComponent.h"

DriveComponent::DriveComponent(vehicleInput* newInput, physx::PxVehicleDrive4W* newVehicle) : Component(DRIVE) {
    myInput = newInput;
    myVehicle = newVehicle;
};

void DriveComponent::setInputs(float accel, float reverse, float handbrake, float steering, bool flip, bool shoot) {
    myInput->accel = accel;
	myInput->reverse = reverse;
	myInput->steering = steering;
	myInput->handbrake = handbrake;
	myInput->flip = flip;
	myInput->shoot = shoot;
}

void DriveComponent::setInputs(vehicleInput in) {
    setInputs(in.accel, in.reverse, in.handbrake, in.steering, in.flip, in.shoot);
}
DriveComponent::~DriveComponent() = default;