#pragma once
#include <map>
#include "../Data/ShaderData.h"

class ShaderDataManager {
private:
	static std::map<std::string, ShaderData*> shaderDataMap;

public:
	ShaderDataManager() = default;
	~ShaderDataManager() = default;
	static ShaderData* getShaderData(std::string materialName);
};
