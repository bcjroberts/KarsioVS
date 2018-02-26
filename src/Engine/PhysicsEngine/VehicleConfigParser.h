#pragma once
#include <vehicle/PxVehicleDrive4W.h>
#include <string>
#include <map>

class VehicleConfigParser
{
private:
    static VehicleConfigParser* globalInstance;
    bool configLoaded = false;
    struct ConfigurablePair
    {
        ConfigurablePair(std::string nname) {
            name = nname;
            value = 0.0f;
        }
        float value;
        std::string name;
    };
    std::map<std::string, ConfigurablePair*> configurableFields;
public:
    VehicleConfigParser();
    ~VehicleConfigParser();
    static VehicleConfigParser* getInstance();
    void parseConfigFile();
    void applyConfigToVehicle(physx::PxVehicleDrive4W* vehicle);

};

