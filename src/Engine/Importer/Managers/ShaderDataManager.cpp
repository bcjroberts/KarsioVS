#include "ShaderDataManager.h"

std::map<std::string, ShaderData*> ShaderDataManager::shaderDataMap;

ShaderData* ShaderDataManager::getShaderData(std::string shaderName) {
	ShaderData* result = shaderDataMap[shaderName];
	if (!result) {
		result = new ShaderData();
		result->loadShader("data/configs/shaders/" + shaderName + ".ini");
		shaderDataMap.insert_or_assign(shaderName, result);
	}
//	std::cout << shaderName << std::endl;
	return result;
}
