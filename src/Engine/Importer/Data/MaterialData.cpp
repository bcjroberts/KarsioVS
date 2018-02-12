// This has all the material data for a model
// aka shader and texture data
//

#include "MaterialData.h"


void MaterialData::addShader(std::string path, GLenum type) {
	shaderData.attachShader(path, type);
}

void MaterialData::clearShader() {
//	ShaderData newShader;
	shaderData = ShaderData();
}

MaterialData::MaterialData() {
	addShader("data/assets/shaderData/default.vert", GL_VERTEX_SHADER);
	addShader("data/assets/shaderData/default.frag", GL_FRAGMENT_SHADER);
	linkShader();
	// Define default shader here

	// Define default textures here
}

void MaterialData::linkShader() {
	shaderData.link();
}
