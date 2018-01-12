//
// Created by Sebastian on 12/23/2017.
//

#ifndef RENDERENGINE_OPENGL_DEPENDENCIES_H
#define RENDERENGINE_OPENGL_DEPENDENCIES_H

//Modified to do it for both Windows and non-windows.
#ifdef _WIN32
#include <GL/glew.h>
#else
#define GLFW_INCLUDE_GLCOREARB
#define GL_GLEXT_PROTOTYPES
#endif
//Include glfw
#include <GLFW/glfw3.h>

//Giving this files since that way my IDE know what I'm trying to access and auto completes the include for me.
class OpenGL_Deps{
    OpenGL_Deps() = default;
    ~OpenGL_Deps() = default;

};

#endif //RENDERENGINE_OPENGL_DEPENDENCIES_H
