//
// Created by Sebastian on 12/23/2017.
//

#include <iostream>
#include "RenderEngine.h"

using namespace glm;

RenderEngine::RenderEngine(GLFWwindow *window) {
    this->window = window;
	for (int i = 0; i < NUM_LIGHTS; ++i) {
		Light newLight;
		newLight.position = vec3(0);
		newLight.color = vec3(0);
		lights.push_back(newLight);
	}

	// Sample code for setting up lights. Currenly only 10 lights are supported.
	setLight(0, vec3(10.0, 10.0, 10.0), vec3(100.0, 100.0, 100.0));
	setLight(1, vec3(20.0, 10.0, 10.0), vec3(100.0, 70.0, 100.0));
	setLight(2, vec3(20.0, 10.0, 20.0), vec3(100.0, 70.0, 20.0));
	setLight(3, vec3(20.0, 20.0, 10.0), vec3(40.0, 70.0, 100.0));
}

RenderEngine::~RenderEngine() {

}

void RenderEngine::render(Camera camera) {

    // clear screen to a dark grey colour
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderElements(camera);


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

const void RenderEngine::setShaderVec3(GLuint shaderID, const std::string& name, const glm::vec3& value) {
	glUniform3fv(glGetUniformLocation(shaderID, name.c_str()), 1, &value[0]);
}
const void RenderEngine::setShaderInt(GLuint shaderID, const std::string &name, int value) {
	glUniform1i(glGetUniformLocation(shaderID, name.c_str()), value);
}

void RenderEngine::passLights(GLuint shaderID) {
	for (int i = 0; i < lights.size(); i++) {
		std::string lightsPos = "lights[" + std::to_string(i) + "].position";
		std::string lightsColor = "lights[" + std::to_string(i) + "].color";
		setShaderVec3(shaderID, lightsPos, lights[i].position);
		setShaderVec3(shaderID, lightsColor, lights[i].color);
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
			setShaderInt(currentShaderID, "textureData.albedo", 0);
			setShaderInt(currentShaderID, "textureData.roughness", 1);
			setShaderInt(currentShaderID, "textureData.metalness", 2);
			setShaderInt(currentShaderID, "textureData.normal", 3);
			setShaderInt(currentShaderID, "textureData.emission", 4);
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
