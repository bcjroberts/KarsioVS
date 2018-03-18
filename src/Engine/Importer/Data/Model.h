// This stores all the data associated with a given model
#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "../../../Main/initOpenGL/OpenGL_Deps.h"



//Include glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//#include "MeshData.h"
//#include "ShaderData.h"
#include "MaterialData.h"
#include "ModelData.h"

//using namespace glm;

class Model {
private:
	void readFile(std::ifstream& file);

public:
	ModelData modelData;
	MaterialData materialData;

	Model() = default;
    ~Model()= default;

    void loadModel(std::string filepath);
};
