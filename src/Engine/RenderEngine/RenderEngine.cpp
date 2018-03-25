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

	struct Label {
		std::string name; // or enum, bascially something by which identifying them will be trival
		int index;
	};
	std::vector<Label> labels;
	Label temp_label;
	temp_label.name = "health";
	temp_label.index = ui->addText("health", 40, 40, 1, vec3(0.5, 1, 0));
	labels.push_back(temp_label);

	temp_label.name = "resources";
	ui->addText("resources", 1550, 30, 0.5, vec3(1, 1, 0));
	labels.push_back(temp_label);

	// Text headers
	temp_label.name = "upgrades";
    temp_label.index = ui->addText("Chassis Lvl: 1", 350, 30, 0.5, vec3(1, 1, 0));
    temp_label.index = ui->addText("Armor Lvl: 1", 650, 30, 0.5, vec3(1, 1, 0));
    temp_label.index = ui->addText("Gun Lvl: 1", 950, 30, 0.5, vec3(1, 1, 0));
    temp_label.index = ui->addText("Ram Lvl: 1", 1250, 30, 0.5, vec3(1, 1, 0));



	// Level indicators
    temp_label.index = ui->addText("", 350, 50, 0.5, vec3(1, 1, 0));
    temp_label.index = ui->addText("", 650, 50, 0.5, vec3(1, 1, 0));
    temp_label.index = ui->addText("", 950, 50, 0.5, vec3(1, 1, 0));
    temp_label.index = ui->addText("", 1250, 50, 0.5, vec3(1, 1, 0));

	// used for physx cycles fps
	ui->addText("78", 5, 5, 0.5, vec3(0, 1, 0));
    // used for cycles through the main loop
    ui->addText("100", 50, 5, 0.5, vec3(1, 1, 0));


	// The same is true for images as for text.
	struct uiImage {
		std::string name; // or enum, bascially something by which identifying them will be trival
		int index; 
	};
	std::vector<uiImage> uiImages;
	uiImage temp;
	temp.name = "hud images";
	temp.index=ui->addImage(*TextureDataManager::getImageData("testSquareImage.jpg"), 1800, 800, 0.1);
	temp.index=ui->addImage(*TextureDataManager::getImageData("testTallerImage.jpg"), 960, 270, 0.25);
	temp.index=ui->addImage(*TextureDataManager::getImageData("testWiderImage.jpg"), 100, 25, 0.25);

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
