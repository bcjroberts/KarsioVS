//
// This file contains the main functions and window callbacks for the game
//

#include "mainGame.h"

int main(){
    OpenGL openGL_Program(screenWidth,screenHeight,"Project Karsio");
	GLFWwindow *window = openGL_Program.getWindow();

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	// Maybe add the other callbacks here too?

    Core core(&screenWidth,&screenHeight, window,false);
    core.coreLoop();

    // final thing that will be printed to inform the programmer that stuff didn't crash ungracefully
    std::cout<<"exiting project"<<std::endl;
    return 0;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
	glViewport(0, 0, width, height);
	screenWidth = width;
	screenHeight = height;
}