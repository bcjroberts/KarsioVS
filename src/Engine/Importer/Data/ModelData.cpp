//
// loads models from a config file
//

#include <fstream>
#include "ModelData.h"



ModelData::ModelData() = default;

void ModelData::importMesh(std::string meshName) {
	std::string meshPath = "data/assets/meshes/" + meshName;
	// Create an instance of the Importer class
	Assimp::Importer importer;

	// And have it read the given file with some example postprocessing
	// Usually - if speed is not the most important aspect for you - you'll 
	// propably to request more postprocessing than we do in this example.
	const aiScene* scene = importer.ReadFile(meshPath,
											 aiProcess_Triangulate | 
											 aiProcess_FlipUVs | 
											 aiProcess_CalcTangentSpace |
											 aiProcess_JoinIdenticalVertices);
	if(!scene) {
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
	}
	
	// process ASSIMP's root node recursively
	processNode(scene->mRootNode, scene);
}

// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
void ModelData::processNode(aiNode *node, const aiScene *scene)
{
	// process each mesh located at the current node
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		// the node object only contains indices to index the actual objects in the scene.
		// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(MeshData(mesh));
	}
	// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene);
	}

}

void ModelData::loadModelData(std::string filepath) {
//	std::string filename = filepath;
	// try to open the file
	std::ifstream file;
	file.open(filepath, std::fstream::in);
	// didn't work? fail!
	if (file.fail()) {
		std::cout << "ERROR: ModelConfig: Couldn't load: " 
		<< filepath << std::endl;
	}

	std::string line; // a buffer to store lines in, and separators
//	const char* values;
	//Process the file
	while (getline(file, line)) {	// while we have data, read in a line
		if(line.substr(0,1) == "#") {
			//if comment don't bother checking anything else and just go to the next line
		}
		else if (line.substr(0, 4) == "Me: ") {
			// import the mesh
			std::string meshName = line.substr(4, line.size());
			importMesh(meshName);
//			std::cout << meshes.size() << std::endl;
//			std::cout << meshes[0].vertices[0].position.x << std::endl;
//			std::cout << meshName << std::endl;
		}
		else if (line.substr(0, 1) == "S") {

			//Include shader based on the kind
			// link when no more shaders -> check for Sq}; or something
		}
		else if (line.substr(0,1) == "T") {
			//Add texture
			//Check which kind should be added
		}
	}
}