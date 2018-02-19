// This has all the material data for a model
// aka shader and texture data
//

#include "MaterialData.h"
#include "../Managers/ShaderDataManager.h"

void MaterialData::loadMaterial(std::string path) {
	//	std::string filename = filepath;
	// try to open the file
	std::ifstream file;
	file.open(path, std::fstream::in);
	// didn't work? fail!
	if (file.fail()) {
		std::cout << "ERROR: ModelConfig: Couldn't load: "
			<< path << std::endl;
	}
	readFile(file);
}


void MaterialData::clearShader() {
//	ShaderData newShader;
	shaderData = ShaderData();
}

//Process the file
void MaterialData::readFile(std::ifstream& file) {

	std::string line; // a buffer to store lines in, and separators
	std::string comment = ";";
	std::string shader = "shader=";
	//TODO:: figure out which textures should be looked for
	std::string diffuse = "diffuse=";
	std::string roughness = "roughness=";
	std::string normal = "normal=";

	while (getline(file, line)) {	// while we have data, read in a line
		if ((line.size() >= comment.size()) && (line.substr(0, comment.size()) == comment))
		{
			//if comment don't bother checking anything else and just go to the next line
		}
		else if ((line.size() >= shader.size()) && (line.substr(0, shader.size()) == shader))
		{
			// import the mesh
			std::string shaderName = line.substr(shader.size(), line.size());
			shaderData = *ShaderDataManager::getShaderData(shaderName);
		}
		//todo::parse the texture data
	}
}
