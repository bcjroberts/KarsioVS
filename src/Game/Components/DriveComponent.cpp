#include "DriveComponent.h"

DriveComponent::DriveComponent(physx::PxVehicleDrive4WRawInputData* newInput) : Component(DRIVE) {
    myInput = newInput;
};

void DriveComponent::setInputs(const bool inAccel, const bool inBrake, const bool inHandbrake, const bool inSteerLeft, const bool inSteerRight, const bool inGearUp, const bool inGearDown) {
    accel = inAccel;
    brake = inBrake;
    handbrake = inHandbrake;
    steerLeft = inSteerLeft;
    steerRight = inSteerRight;
    gearUp = inGearUp;
    gearDown = inGearDown;

    // Setting of the input right here
    myInput->setDigitalBrake(brake);
    myInput->setDigitalAccel(accel);
    myInput->setDigitalSteerLeft(steerLeft);
    myInput->setDigitalSteerRight(steerRight);
    myInput->setDigitalHandbrake(handbrake);
    myInput->setGearDown(gearDown);
    myInput->setGearUp(gearUp);
}

void DriveComponent::setInputs(vehicleInput in) {
    accel = in.accel;
    brake = in.brake;
    handbrake = in.handbrake;
    steerLeft = in.steerLeft;
    steerRight = in.steerRight;
    gearUp = in.gearUp;
    gearDown = in.gearDown;

    // Setting of the input right here
    myInput->setDigitalBrake(in.brake);
    myInput->setDigitalAccel(in.accel);
    myInput->setDigitalSteerLeft(in.steerLeft);
    myInput->setDigitalSteerRight(in.steerRight);
    myInput->setDigitalHandbrake(in.handbrake);
    myInput->setGearDown(in.gearDown);
    myInput->setGearUp(in.gearUp);
}
DriveComponent::~DriveComponent() = default;