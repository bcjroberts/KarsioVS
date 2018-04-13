//
// main render engine that has pointers to all the other 
// rendering related stuff like ui and render world
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

	world = new RenderWorld(screenWidth, screenHeight);
	ui = new UserInterface(screenWidth,screenHeight);

	//Maybe load the font from somewhere else?
	ui -> loadFont(Core::dataPath + "assets/fonts/duralith/DURALITH.ttf", 48);
}

RenderEngine::~RenderEngine() {

}

void RenderEngine::render(std::vector<Camera*> cameras) {

    // clear screen to a dark grey colour
//    glClearColor(10.f/255.f, 18.f/255.f, 30.f/255.f, 1.0f);
    glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (int i = 0; i < cameras.size(); ++i) {
		world->updateCamera(cameras[i]->getView(), cameras[i]->getProjection(), cameras[i]->getPosition());
		world->renderScene();

		ui->renderUI();
	}

    glUseProgram(0); //cleanup

    // Swap the screen buffers
    glfwSwapBuffers(window);
}
