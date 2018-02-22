#include "ModelData.h"



void ModelData::importMesh(std::string meshPath) {
	// Create an instance of the Importer class
	Assimp::Importer importer;

	// And have it read the given file with some example postprocessing
	// Usually - if speed is not the most important aspect for you - you'll 
	// propably to request more postprocessing than we do in this example.
	const aiScene* scene = importer.ReadFile(meshPath,
											 aiProcess_Triangulate | // Ensure all verticies are triangulated (each 3 vertices are triangle)
											 aiProcess_FlipUVs |
											 aiProcess_CalcTangentSpace | // calculate tangents and bitangents if possible
											 aiProcess_JoinIdenticalVertices | // join identical vertices/ optimize indexing
											 aiProcess_ImproveCacheLocality | // improve the cache locality of the output vertices
											 aiProcess_FindInstances | // search for instanced meshes and remove them by references to one master
											 aiProcess_OptimizeMeshes | // join small meshes, if possible;
											 0);
	if (!scene) {
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
	}

	// process ASSIMP's root node recursively
	processNode(scene->mRootNode, scene);
}

// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
void ModelData::processNode(aiNode *node, const aiScene *scene) {
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