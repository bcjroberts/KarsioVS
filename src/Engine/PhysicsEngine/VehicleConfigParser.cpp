#include "VehicleConfigParser.h"
#include <fstream>

VehicleConfigParser *VehicleConfigParser::globalInstance = nullptr;

VehicleConfigParser::VehicleConfigParser()
{
    // Insert an element for every value that is configurable
    configurableFields.insert_or_assign("enginePeakTorque", new ConfigurablePair("enginePeakTorque"));
    configurableFields.insert_or_assign("engineMaxOmega", new ConfigurablePair("engineMaxOmega"));
};
VehicleConfigParser::~VehicleConfigParser() = default;

// Can globally access the EntityManager from anywhere

VehicleConfigParser *
VehicleConfigParser::getInstance()
{
    if (!globalInstance)
    {
        globalInstance = new VehicleConfigParser;
    }
    return globalInstance;
}

void VehicleConfigParser::parseConfigFile()
{
    // This parses the config file for vehicles, and updates the local struct with the values
    std::ifstream file;
    file.open("data/configs/car/default.ini", std::fstream::in);
    if (file.fail())
    {
        printf("ERROR: Failed to load the default car config file...");
        configLoaded = false;
        return;
    }
    std::string currentLine;
    std::string comment = ";";
    const char splitter = '=';

    while (getline(file, currentLine))
    {
        // Skip comments and empty lines
        if (currentLine.size() <= comment.size() || currentLine.substr(0, comment.size()) == comment)
        {
            continue;
        }

        // Check the value against our dictionary, and insert it if found. Otherwise show a warning
        const int splitLocation = currentLine.find(splitter);
        const std::string name = currentLine.substr(0, splitLocation);
        const float value = std::stof(currentLine.substr(splitLocation+1, currentLine.size()));

        // ensure the element exists before attempting to access and modify it
        if (configurableFields.count(name) != 0)
            configurableFields[name]->value = value;
    }
    configLoaded = true;
}

void VehicleConfigParser::applyConfigToVehicle(physx::PxVehicleDrive4W* vehicle) {
    if (!configLoaded) {
        printf("WARNING: No new config was loaded, vehicle not updated.");
        return;
    }

    // Now set all of the configurable stuff for the vehicle here
    physx::PxVehicleDriveSimData4W* driveSimData = &vehicle->mDriveSimData;
    physx::PxVehicleEngineData engineData = driveSimData->getEngineData();
    engineData.mMaxOmega = configurableFields["engineMaxOmega"]->value;
    engineData.mPeakTorque = configurableFields["enginePeakTorque"]->value;
    driveSimData->setEngineData(engineData);
}
