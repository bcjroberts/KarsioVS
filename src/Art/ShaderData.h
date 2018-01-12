//
// Created by Sebastian on 06/01/2018.
//

#ifndef PROJECTKARSIO_SHADERS_H
#define PROJECTKARSIO_SHADERS_H

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

#include "../Main/initOpenGL/OpenGL_Deps.h"

class ShaderData {
public:
//private:
    GLuint id = 0;
//public:
    ShaderData();
    ~ShaderData();
    bool attachShader(std::string fileName, GLenum type);
    bool link();
    bool use();
    static void unbind();
};


#endif //PROJECTKARSIO_SHADERS_H
