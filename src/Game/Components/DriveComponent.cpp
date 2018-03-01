#include "DriveComponent.h"

DriveComponent::DriveComponent(physx::PxVehicleDrive4WRawInputData* newInput, physx::PxVehicleDrive4W* newVehicle) : Component(DRIVE) {
    myInput = newInput;
    myVehicle = newVehicle;
};

void DriveComponent::setInputs(const float accel, const float brake, const float handbrake, const float steering, const bool flip) {
    this->accel = accel;
    this->brake = brake;
    this->steering = steering;
    this->handbrake = handbrake;
    this->flip = flip;

    myInput->setAnalogAccel(accel);
    myInput->setAnalogBrake(brake);
    myInput->setAnalogHandbrake(handbrake);
    myInput->setAnalogSteer(steering);
}

void DriveComponent::setInputs(vehicleInput in) {
    setInputs(in.accel, in.brake, in.handbrake, in.steering, in.flip);
}
DriveComponent::~DriveComponent() = default;