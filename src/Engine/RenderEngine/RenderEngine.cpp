//
// Created by Sebastian on 12/23/2017.
//

#include <iostream>
#include "RenderEngine.h"
#include "ShaderUniforms.h"
#include "../Core.h"
#include "../Importer/Managers/TextureDataManager.h"
//#include "UserInterface.h"

using namespace glm;

RenderEngine::RenderEngine(GLFWwindow *window, int *screenWidth, int *screenHeight) {
    this->window = window;
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;

	world = new RenderWorld();
	ui = new UserInterface(screenWidth,screenHeight);

	//Maybe load the font from somewhere else?
	ui -> loadFont(Core::dataPath + "assets/fonts/duralith/DURALITH.ttf", 48);
	//==========================================================
	//replace all of this with stuff that isn't in the render engine 
	// then remove it from the project before submission
	//==========================================================

	// Adding the following example code since _nobody_ noticed that you 
	//  can get the index of the label upon it's creation and it'd be nice to get
	//  ride of the magic numbers in the core...
	//  
	// Also please move this out of the render engine and into something more approperate, 
	//  I'm not too familar with how all of this is being used so I don't want to break things 
	//  or clutter the core. Ideally there'll be a seperate class for all the strings which will 
	//  be easily accessible.

	//==========================================================
	//\End of temp section
	//==========================================================
}

RenderEngine::~RenderEngine() {

}

void RenderEngine::render(std::vector<Camera*> cameras) {

    // clear screen to a dark grey colour
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (int i = 0; i < cameras.size(); ++i) {
		world->updateCamera(cameras[i]->getView(), cameras[i]->getProjection(), cameras[i]->getPosition());
		world->renderElements();

		ui->renderUI();
	}

    glUseProgram(0); //cleanup

    // Swap the screen buffers
    glfwSwapBuffers(window);
}
