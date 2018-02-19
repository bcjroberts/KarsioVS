// Manages all the models that were added to the game
#pragma once
#include "../Data/Model.h"
#include <map>


class ModelManager {
private:
    static std::map<std::string, Model*> modelMap;

public:
    ModelManager();
    ~ModelManager();
    static Model* getModel(std::string modelName);
};

