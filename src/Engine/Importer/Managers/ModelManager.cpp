#include "ModelManager.h"
#include "../../Core.h"

ModelManager::ModelManager() = default;
ModelManager::~ModelManager() = default;
std::map<std::string, Model*> ModelManager::modelMap;

// Retrieves the mesh data either by returning an existing reference or creating a new one.
Model* ModelManager::getModel(std::string modelName) {
    if (loadOnlyBoxes && modelName != "plane") {
        modelName = "cube";
    }
    Model* result = modelMap[modelName];
    if (!result) {
        result = new Model();
        result->loadModel(Core::dataPath + "configs/models/" + modelName + ".ini");
        modelMap.insert_or_assign(modelName, result);
    }
//	std::cout << modelName << std::endl;
    return result;
}
