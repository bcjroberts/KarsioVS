//
// Created by Sebastian on 06/01/2018.
//

#ifndef PROJECTKARSIO_SHADERS_H
#define PROJECTKARSIO_SHADERS_H

#include <fstream>
#include <iostream>
#include <string>

#include "../../../Main/initOpenGL/OpenGL_Deps.h"

class ShaderData {
private:
	void attachShader(std::string shaderPath, GLenum type);
	void link();
	void readFile(std::ifstream &file);

public:
    GLuint shaderID = 0;
	ShaderData() = default;
    ~ShaderData();
	void loadShader(std::string filepath);
};


#endif //PROJECTKARSIO_SHADERS_H
