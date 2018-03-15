//
// Created by Sebastian on 12/23/2017.
//

#include <iostream>
#include "RenderEngine.h"
#include "ShaderUniforms.h"
#include "../Core.h"
//#include "UserInterface.h"

using namespace glm;

RenderEngine::RenderEngine(GLFWwindow *window, int *screen_width, int *screen_height) {
    this->window = window;
	for (int i = 0; i < NUM_LIGHTS; ++i) {
		Light newLight;
		newLight.position = vec3(0);
		newLight.color = vec3(0);
		lights.push_back(newLight);
        lightsInUse[i] = false;
	}
	this->screenWidth = screen_width;
	this->screenHeight = screen_height;
	ui = new UserInterface(screenWidth,screenHeight);
	//Maybe load the font from somewhere else?
	ui -> loadFont(Core::dataPath + "assets/fonts/duralith/DURALITH.ttf", 48);
	//==========================================================
	//replace all of this with stuff that isn't in the render engine and then remove it from the project before submission
	//==========================================================
	ui->addText("health", 40, 40, 1, vec3(0.5, 1, 0));
	ui->addText("resources", 1550, 30, 0.5, vec3(1, 1, 0));

	// Text headers
    ui->addText("Chassis Lvl: 1", 350, 30, 0.5, vec3(1, 1, 0));
    ui->addText("Armor Lvl: 1", 650, 30, 0.5, vec3(1, 1, 0));
    ui->addText("Gun Lvl: 1", 950, 30, 0.5, vec3(1, 1, 0));
    ui->addText("Ram Lvl: 1", 1250, 30, 0.5, vec3(1, 1, 0));

	// Level indicators
    ui->addText("", 350, 50, 0.5, vec3(1, 1, 0));
    ui->addText("", 650, 50, 0.5, vec3(1, 1, 0));
    ui->addText("", 950, 50, 0.5, vec3(1, 1, 0));
    ui->addText("", 1250, 50, 0.5, vec3(1, 1, 0));

	// fps
	ui->addText("78", 5, 5, 0.5, vec3(1, 1, 0));

	//ui->addText("Try to use relative values that can be dynamically changed for easy resizing", (*screenWidth)/4, (*screenHeight)/2, 0.5, vec3(0.75, 1, 0.5));
	//ui->addText("This sample code exists in the render engine's constructor, look at it to figure out how this works", (*screenWidth)/4, 3*(*screenHeight)/4, 0.5, vec3(0.75, 1, 0.5));
	//ui->removeText(2);
	// Sample code for setting up lights. Currenly only 10 lights are supported.
	setLight(0, vec3(20.0, 10.0, 20.0), vec3(1000.0, 1000.0, 1000.0));
	setLight(1, vec3(-20.0, 10.0, -20.0), vec3(1000.0, 1000.0, 1000.0));
	setLight(2, vec3(-20.0, 10.0, 20.0), vec3(1000.0, 1000.0, 1000.0));
    setLight(3, vec3(20.0, 10.0, -20.0), vec3(1000.0, 1000.0, 1000.0));
    lightsInUse[0] = lightsInUse[1] = lightsInUse[2] = lightsInUse[3] = true;
	//==========================================================
	//\End of temp section
	//==========================================================
}

RenderEngine::~RenderEngine() {

}

void RenderEngine::render(Camera camera) {

    // clear screen to a dark grey colour
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderElements(camera);

	ui->renderTextArray();
//	ui->renderText("Hello world", 5, 5, 1, vec3(0.5,1,0));

    glUseProgram(0); //cleanup

    // Swap the screen buffers
    glfwSwapBuffers(window);
}

void RenderEngine::setLight(int index, vec3 position, vec3 color) {
	Light newLight;
	newLight.position = position;
	newLight.color = color;
	lights[index] = newLight;
}

int RenderEngine::getNextAvailableLightID() {
    for (int i = 0; i < NUM_LIGHTS; ++i) {
        if (!lightsInUse[i]) {
            //printf("Light %i reserved\n", i);
            lightsInUse[i] = true;
            return i;
        }
    }
    return -1;
}

void RenderEngine::freeLightWithID(int index) {
    setLight(index, vec3(0), vec3(0));
    //printf("Light %i freed\n", index);
    lightsInUse[index] = false;
}

//const void RenderEngine::setShaderVec3(GLuint shaderID, const std::string& name, const glm::vec3& value) {
//	glUniform3fv(glGetUniformLocation(shaderID, name.c_str()), 1, &value[0]);
//}
//const void RenderEngine::setShaderInt(GLuint shaderID, const std::string &name, int value) {
//	glUniform1i(glGetUniformLocation(shaderID, name.c_str()), value);
//}

void RenderEngine::passLights(GLuint shaderID) {
	for (int i = 0; i < lights.size(); i++) {
		std::string lightsPos = "lights[" + std::to_string(i) + "].position";
		std::string lightsColor = "lights[" + std::to_string(i) + "].color";
		ShaderUniforms::setVec3(shaderID, lightsPos, lights[i].position);
		ShaderUniforms::setVec3(shaderID, lightsColor, lights[i].color);
	}
}

void RenderEngine::passTextures(RendererModel sModel) {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sModel.texture.albedo);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, sModel.texture.roughness);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, sModel.texture.metalness);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, sModel.texture.normal);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, sModel.texture.emission);
}

void RenderEngine::renderElements(Camera camera) {
    GLuint currentShaderID = -1;
    GLint transformationLoc;
    for (auto &sModel : sceneModels) {
		
        if(sModel.shaderID != currentShaderID){
			currentShaderID = sModel.shaderID;
			glUseProgram(currentShaderID);

			//Pass lights into shader
			passLights(currentShaderID);

			// Camera/View transformation
			GLint viewLoc = glGetUniformLocation(currentShaderID, "view");
			GLint projLoc = glGetUniformLocation(currentShaderID, "projection");
			GLint viewPosLoc = glGetUniformLocation(currentShaderID, "viewPos");
			transformationLoc = glGetUniformLocation(currentShaderID, "modelTransformation");
			camera.setupCameraTransformationMatrices(viewLoc, projLoc, viewPosLoc);

			// setup texture shaders
			ShaderUniforms::setInt(currentShaderID, "textureData.albedo", 0);
			ShaderUniforms::setInt(currentShaderID, "textureData.roughness", 1);
			ShaderUniforms::setInt(currentShaderID, "textureData.metalness", 2);
			ShaderUniforms::setInt(currentShaderID, "textureData.normal", 3);
			ShaderUniforms::setInt(currentShaderID, "textureData.emission", 4);
        }

		passTextures(sModel);

		for (auto &mesh : sModel.meshes) {
			for (int j = 0; j < sModel.instances.size(); ++j) {
				glBindVertexArray(mesh.VAO);
				glUniformMatrix4fv(transformationLoc, 1, GL_FALSE, value_ptr(sModel.instances[j].transform));
				glDrawElements(GL_TRIANGLES, mesh.trisCount, GL_UNSIGNED_INT, 0);
				glBindVertexArray(0);
			}
		}
    }
    //std::cout<<"rendering"<<std::endl;
}
//
//void RenderEngine::loadFont(std::string path, int size) {
//	ui->loadFont(path, size);
//}
//
//int RenderEngine::addText(std::string contents, int xpos, int ypos, int scale, glm::vec3 color) {
//	return ui->addText(contents, xpos, ypos, scale, color);
//}
//
//void RenderEngine::modifyText(int index, std::string* contents, int* xpos, int* ypos, int* scale, glm::vec3* color) {
//	ui->modifyText(0, contents, xpos, ypos, scale, color);
//}

void RenderEngine::addInstance(Model &model, int id, mat4 transform) {
    bool createNewModelGroup = true;
    Instance instance = {id,transform};
    for (int i = 0; i < sceneModels.size(); ++i) {
        if(sceneModels[i].meshes[0].VAO == model.modelData.meshes[0].VAO){
            sceneModels[i].instances.push_back(instance);
            createNewModelGroup = false;
        }
    }
    if(createNewModelGroup){
		RendererModel rendererModel; //deep copy values from main model data to render specific model list
		rendererModel.shaderID = model.materialData.shaderID;
		rendererModel.texture.albedo = model.materialData.texture.albedo;
		rendererModel.texture.roughness = model.materialData.texture.roughness;
		rendererModel.texture.metalness = model.materialData.texture.metalness;
		rendererModel.texture.normal = model.materialData.texture.normal;
		rendererModel.texture.emission = model.materialData.texture.emission;

//		model.geometry.makeBuffer(model);
		for (int i = 0; i < model.modelData.meshes.size(); i++) {
			Geometry mesh;
			mesh.VAO = model.modelData.meshes[i].VAO;
			mesh.trisCount = model.modelData.meshes[i].indices.size();
			rendererModel.meshes.push_back(mesh);
		}
		rendererModel.instances.push_back(instance);

        sceneModels.push_back(rendererModel);
    }
}

void RenderEngine::updateInstance(Model &model, int id, mat4 transform) {
    for (int i = 0; i < sceneModels.size(); ++i) {
        if (sceneModels[i].meshes[0].VAO == model.modelData.meshes[0].VAO) {
            for (int j = 0; j < sceneModels[i].instances.size(); ++j) {
                if(sceneModels[i].instances[j].ID==id){
                    sceneModels[i].instances[j].transform = transform;
                }
            }
        }
    }
}

void RenderEngine::removeInstance(Model &model, int id) {
    for (int i = 0; i < sceneModels.size(); ++i) {
        if (sceneModels[i].meshes[0].VAO == model.modelData.meshes[0].VAO) {
            for (int j = 0; j < sceneModels[i].instances.size(); ++j) {
                if (sceneModels[i].instances[j].ID == id) {
                    sceneModels[i].instances.erase(sceneModels[i].instances.begin() + j);
                }
            }
        }
    }
}
