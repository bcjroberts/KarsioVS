#include "ModelDataManager.h"
std::map<std::string, ModelData*> ModelDataManager::modelDataMap;

ModelData* ModelDataManager::getModelData(std::string modelDataName) {
	ModelData* result = modelDataMap[modelDataName];
	if (!result) {
		result = new ModelData();
		result->importMesh("data/assets/meshes/" + modelDataName);
		modelDataMap.insert_or_assign(modelDataName, result);
	}
	std::cout << modelDataName << std::endl;
	return result;
}
