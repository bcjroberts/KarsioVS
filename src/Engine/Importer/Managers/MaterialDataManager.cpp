#include "MaterialDataManager.h"
std::map<std::string, MaterialData*> MaterialDataManager::materialDataMap;

MaterialData* MaterialDataManager::getMaterialData(std::string materialName) {
	MaterialData* result = materialDataMap[materialName];
	if (!result) {
		result = new MaterialData();
		result->loadMaterial("data/configs/materials/" + materialName + ".ini");
		materialDataMap.insert_or_assign(materialName, result);
	}
	std::cout << materialName << std::endl;
	return result;
}
