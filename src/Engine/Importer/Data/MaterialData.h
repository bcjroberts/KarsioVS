// This has all the material data for a model
// aka shader and texture data
//
#pragma once

#include "ShaderData.h"
#include "TextureData.h"

class MaterialData {
private:
	void readFile(std::ifstream& file);
	
public:
	ShaderData shaderData;
//	GLuint shaderDataID;
	struct Texture {
		TextureData albedo;
		TextureData roughness;
		TextureData texture1;
		TextureData texture2;
		TextureData texture3;
	};
	void loadMaterial(std::string path);

	void addTexture();
	void clearShader();
	MaterialData() = default;
	~MaterialData() = default;
};


