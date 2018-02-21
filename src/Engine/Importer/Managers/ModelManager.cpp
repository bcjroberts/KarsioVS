#include "ModelManager.h"

ModelManager::ModelManager() = default;
ModelManager::~ModelManager() = default;
std::map<std::string, Model*> ModelManager::modelMap;

// Retrieves the mesh data either by returning an existing reference or creating a new one.
Model* ModelManager::getModel(std::string modelName) {
    Model* result = modelMap[modelName];
    if (!result) {
        result = new Model();
        result->loadModel("data/configs/models/" + modelName + ".ini");
        modelMap.insert_or_assign(modelName, result);
    }
//	std::cout << modelName << std::endl;
    return result;
}
