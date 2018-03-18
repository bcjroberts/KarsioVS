//
// loads models from a config file
//

#include <fstream>
#include "Model.h"
#include "../Managers/ModelDataManager.h"
#include "../Managers/MaterialDataManager.h"

//using namespace glm;

void Model::loadModel(std::string filepath) {
	//	std::string filename = filepath;
	// try to open the file
	std::ifstream file;
	file.open(filepath, std::fstream::in);
	// didn't work? fail!
	if (file.fail()) {
		std::cout << "ERROR: ModelConfig: Couldn't load: "
			<< filepath << std::endl;
	}
	readFile(file);
}

//Process the file
void Model::readFile(std::ifstream& file) {
	std::string line; // a buffer to store lines in, and separators
	std::string comment = ";";
	std::string model = "model=";
	std::string mat = "material=";

	while (getline(file, line)) {	// while we have data, read in a line
		if ((line.size()>=comment.size()) && (line.substr(0, comment.size()) == comment))
		{
			//if comment don't bother checking anything else and just go to the next line
		}
		else if ((line.size() >= model.size()) && (line.substr(0, model.size()) == model))
		{
			// import the mesh
			std::string meshName = line.substr(model.size(), line.size());
			modelData = *ModelDataManager::getModelData(meshName);
		}
		else if ((line.size() >= mat.size()) && (line.substr(0, mat.size()) == mat))
		{
			// import the mesh
			std::string materialName = line.substr(mat.size(), line.size());
			materialData = *MaterialDataManager::getMaterialData(materialName);
		}
	}
}
