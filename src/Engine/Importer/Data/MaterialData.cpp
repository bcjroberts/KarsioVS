// This has all the material data for a model
// aka shader and texture data
//

#include "MaterialData.h"
#include "../Managers/ShaderDataManager.h"
#include "../Managers/TextureDataManager.h"

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

MaterialData::MaterialData() {
	// Create sensible default settings for texture data
	// If proper textures are provided these will be overwritten.
	// The way this is set up allows for one to only specify the textures they 
	//  don't want to leave at default.
	texture.albedo = TextureDataManager::defaultAlbedo();
	texture.roughness = TextureDataManager::defaultRoughness();
	texture.metalness = TextureDataManager::defaultMetalness();
	texture.normal = TextureDataManager::defaultNormal();
	texture.emission = TextureDataManager::defaultEmission();
	
//	std::cout << texture.albedo << std::endl;
//	std::cout << texture.roughness << std::endl;
//	std::cout << texture.metalness << std::endl;
//	std::cout << texture.normal << std::endl;
}

//Process the file
void MaterialData::readFile(std::ifstream& file) {

	std::string line; // a buffer to store lines in, and separators
	std::string comment = ";";
	std::string shader = "shader=";

	//The following textures will be looked for:
	// Might want to include a texture to control fresnel rather than 
	// just have metalness determine whether 0.04 or the albedo are used
	std::string albedo = "albedo=";
	std::string roughness = "roughness="; 
	std::string metalness = "metalness="; 
	std::string normal = "normal=";
	std::string emission = "emission=";
	
	while (getline(file, line)) {	// while we have data, read in a line
		if ((line.size() >= comment.size()) && (line.substr(0, comment.size()) == comment))
		{
			//if comment don't bother checking anything else and just go to the next line
		}
		else if ((line.size() >= shader.size()) && (line.substr(0, shader.size()) == shader))
		{
			// import the mesh
			std::string shaderName = line.substr(shader.size(), line.size());
//			shaderData = *ShaderDataManager::getShaderData(shaderName);
			shaderID = ShaderDataManager::getShaderData(shaderName)->shaderID;
		}
		//todo::parse the texture data from the texture files
	}
}
