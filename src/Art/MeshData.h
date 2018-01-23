//
// Created by Sebastian on 1/4/2018.
//

#ifndef PROJECTKARSIO_MODEL_H
#define PROJECTKARSIO_MODEL_H
#include <iostream>
#include <string>
#include <vector>

#include "../Main/initOpenGL/OpenGL_Deps.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//Include glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

struct Vertex {
    // Position
    glm::vec3 position;
    // Normal
    glm::vec3 normal;
    // TexCoords
    glm::vec2 uvCoords;
};

class MeshData {
private:
    std::string model_ID;
    //This parses and loads the model data from a file
    //void loadModelData(std::string filepath);

    //This is a temp function that will load a subset of possible obj files
    // the obj file needs to have one single mesh, a single uv map and triangulated faces
    void openOBJ(std::string filename);

public:
    //MeshData *meshDataPointer;
    std::vector<Vertex> vertices;
    //std::vector<int> vertexIndices, uvIndices, normalIndices;
    //std::vector<vec3> vertices;
    //std::vector<vec3> normals;
    //std::vector<vec2> uvs;


    MeshData(std::string name);
    ~MeshData()= default;

    void loadMeshData(std::string filepath);
    //void addTexture(std::string filepath);

    //void processNode(aiNode *node, const aiScene *scene);
};


#endif //PROJECTKARSIO_MODEL_H
