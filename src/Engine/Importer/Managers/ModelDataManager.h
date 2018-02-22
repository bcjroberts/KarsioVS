#pragma once
#include "../Data/ModelData.h"
#include <map>

class ModelDataManager {
private:
	static std::map<std::string, ModelData*> modelDataMap;
	
public:
	ModelDataManager() = default;
	~ModelDataManager() = default;
	static ModelData* getModelData(std::string modelDataName);
};
