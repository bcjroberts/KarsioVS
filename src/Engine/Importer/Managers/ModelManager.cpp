#include "ModelManager.h"

ModelManager::ModelManager() = default;
ModelManager::~ModelManager() = default;
std::map<std::string, ModelData*> ModelManager::modelDataMap;

// Retrieves the mesh data either by returning an existing reference or creating a new one.
ModelData* ModelManager::getModelData(std::string modelName) {
    ModelData* result = modelDataMap[modelName];
    if (!result) {
        result = new ModelData();
        result->loadModelData("data/assets/models/" + modelName + ".txt");
        modelDataMap.insert_or_assign(modelName, result);
    }
    return result;
}
