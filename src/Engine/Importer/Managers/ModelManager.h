// Manages all the models that were added to the game
#pragma once
#include "../Data/ModelData.h"
#include <map>
#include "TextureManager.h"
#include "ShaderManager.h"
#include "MeshManager.h"

class ModelManager {
private:
    static std::map<std::string, ModelData*> modelDataMap;
	MeshManager meshManager;
	ShaderManager shaderManager;
	TextureManager textureManager;
public:
    ModelManager();
    ~ModelManager();
    static ModelData* getModelData(std::string modelName);
};

