#include "DriveComponent.h"

DriveComponent::DriveComponent(physx::PxVehicleDrive4WRawInputData* newInput) : Component(DRIVE) {
    myInput = newInput;
};

void DriveComponent::setInputs(const float accel, const float brake, const float handbrake, const float steering) {
    this->accel = accel;
    this->brake = brake;
    this->steering = steering;
    this->handbrake = handbrake;

    myInput->setAnalogAccel(accel);
    myInput->setAnalogBrake(brake);
    myInput->setAnalogHandbrake(handbrake);
    myInput->setAnalogSteer(steering);
}

void DriveComponent::setInputs(vehicleInput in) {
    setInputs(in.accel, in.brake, in.handbrake, in.steering);
}
DriveComponent::~DriveComponent() = default;