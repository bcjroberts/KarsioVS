// This stores all the data associated with a given model

#ifndef PROJECTKARSIO_MODEL_H
#define PROJECTKARSIO_MODEL_H
#include <iostream>
#include <string>
#include <vector>

#include "../../../Main/initOpenGL/OpenGL_Deps.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//Include glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "MeshData.h"
#include "ShaderData.h"
#include "MaterialData.h"
#include "../Managers/TextureManager.h"
#include "../Managers/ShaderManager.h"
#include "../Managers/MeshManager.h"

using namespace glm;

class ModelData {
private:

	void processNode(aiNode *node, const aiScene *scene);
//	std::vector<Vertex> getVertices(aiMesh *mesh);
//	std::vector<unsigned int> getIndices(aiMesh* mesh);
	MeshData processMesh(aiMesh* mesh);

public:

	std::vector<MeshData> meshes;
	MaterialData materialData;
    //ModelData *meshDataPointer;
//    std::vector<Vertex> vertices;
    //std::vector<int> vertexIndices, uvIndices, normalIndices;
    //std::vector<vec3> vertices;
    //std::vector<vec3> normals;
    //std::vector<vec2> uvs;
    ModelData();
    ~ModelData()= default;

	void importMesh(std::string filepath);
    void loadModelData(std::string filepath);
    //void addTexture(std::string filepath);

};


#endif //PROJECTKARSIO_MODEL_H
