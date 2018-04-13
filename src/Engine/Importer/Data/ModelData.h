#pragma once



#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>
#include <string>
#include <vector>

#include "../../../Main/initOpenGL/OpenGL_Deps.h"

#include "MeshData.h"

class ModelData {

private:
	bool isCrystal;
	void processNode(aiNode *node, const aiScene *scene);
	MeshData processMesh(aiMesh* mesh);

public:
	ModelData() = default;
	~ModelData() = default;
	std::vector<MeshData> meshes;
	void importMesh(std::string filepath);
};
