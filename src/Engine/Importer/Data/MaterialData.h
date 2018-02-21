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
//	ShaderData shaderData;
	GLuint shaderID = 0;
//	GLuint shaderDataID;
	struct Texture {
		GLuint albedo;
		GLuint roughness;
		GLuint metalness;
		GLuint normal;
		GLuint emission;
	};
	Texture texture;
	void loadMaterial(std::string path);

	void addTexture();
	MaterialData();
	~MaterialData() = default;
};


