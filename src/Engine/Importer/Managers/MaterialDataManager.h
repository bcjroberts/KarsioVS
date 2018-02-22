#pragma once
#include <map>
#include "../Data/MaterialData.h"

class MaterialDataManager {
private:
	static std::map<std::string, MaterialData*> materialDataMap;

public:
	MaterialDataManager() = default;
	~MaterialDataManager() = default;
	static MaterialData* getMaterialData(std::string materialName);
};
