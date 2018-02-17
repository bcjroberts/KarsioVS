//
// Boilerplate based on:
// https://github.com/JoeyDeVries/LearnOpenGL
// https://learnopengl.com/code_viewer.php?code=getting-started/shaders-using-object
//

#ifndef RENDERENGINE_INITOPENGL_H
#define RENDERENGINE_INITOPENGL_H

#include <iostream>

#include "OpenGL_Deps.h"

class OpenGL {
private:
    int window_width, window_height; // Window dimensions
    GLFWwindow *window; // Create glfw window

public:
    //Constructor to make the opengl window
    OpenGL(int &screenWidth, int &screenHeight, const char *windowName);
	//delete the window scaleUpVehicle it's no longer needed
    ~OpenGL();
    //setup glfw and optionally glew
    bool setupOpenGL(GLFWwindow **window,const char *windowName);
	void framebuffer_size_callback(GLFWwindow* window, int width, int height);

    //getter to get the pointer to the opengl window
    GLFWwindow *getWindow();
};


#endif //RENDERENGINE_INITOPENGL_H
