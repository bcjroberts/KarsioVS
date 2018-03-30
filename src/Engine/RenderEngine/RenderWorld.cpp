﻿#include "RenderWorld.h"
#include "ShaderUniforms.h"
#include "../Importer/Managers/TextureDataManager.h"
#include "../Importer/Managers/ShaderDataManager.h"
#include "../Importer/Managers/ModelDataManager.h"

using namespace glm;

RenderWorld::RenderWorld() {
	for (int i = 0; i < NUM_LIGHTS; ++i) {
		Light newLight;
		newLight.position = vec3(0);
		newLight.color = vec3(0);
		lights.push_back(newLight);
		lightsInUse[i] = false;
	}
	setLight(0, vec3(100.0, 100.0, 100.0), vec3(20000.0, 20000.0, 20000.0));
	setLight(1, vec3(100.0, 100.0, -100.0), vec3(20000.0, 20000.0, 20000.0));
	setLight(2, vec3(-100.0, 100.0, 100.0), vec3(20000.0, 20000.0, 20000.0));
	setLight(3, vec3(-100.0, 100.0, -100.0), vec3(20000.0, 20000.0, 20000.0));
	lightsInUse[0] = lightsInUse[1] = lightsInUse[2] = lightsInUse[3] = true;

	//load the skybox
	loadCubemap();
}

RenderWorld::~RenderWorld() {
}

void RenderWorld::renderElements() {
	GLuint currentShaderID = -1;
	GLint transformationLoc;
	for (auto &sModel : sceneModels) {

		if (sModel.shaderID != currentShaderID) {
			currentShaderID = sModel.shaderID;
			glUseProgram(currentShaderID);

			passLights(currentShaderID); //pass lights into shader
			passCamera(currentShaderID); //pass camera into shader
			passTextures(currentShaderID);

			transformationLoc = glGetUniformLocation(currentShaderID, "modelTransformation");
		}

		activateTextures(sModel);

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

void RenderWorld::renderCubemap() {
	glDepthFunc(GL_LEQUAL);
	glDisable(GL_CULL_FACE);
	glUseProgram(cubemap.shaderID);
	ShaderUniforms::setMat4(cubemap.shaderID, "view", mat4(mat3(cameraDetails.view)));
	ShaderUniforms::setMat4(cubemap.shaderID, "projection", cameraDetails.projection);
//	passCamera(cubemap.shaderID);
	//	view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
	//	skyboxShader.setMat4("view", view);
	//	skyboxShader.setMat4("projection", projection);
	// skybox cube
	glBindVertexArray(cubemap.model.VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap.textureID);
	glDrawElements(GL_TRIANGLES, cubemap.model.trisCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glEnable(GL_CULL_FACE);
	glDepthFunc(GL_LESS); // set depth function back to default
}

void RenderWorld::updateCamera(glm::mat4& view, glm::mat4& projection, glm::vec3& location) {
	cameraDetails.view = view;
	cameraDetails.projection = projection;
	cameraDetails.location = location;
}

void RenderWorld::setLight(int index, vec3 position, vec3 color) {
	Light newLight;
	newLight.position = position;
	newLight.color = color;
	lights[index] = newLight;
}

int RenderWorld::getNextAvailableLightID() {
	for (int i = 0; i < NUM_LIGHTS; ++i) {
		if (!lightsInUse[i]) {
			//printf("Light %i reserved\n", i);
			lightsInUse[i] = true;
			return i;
		}
	}
	return -1;
}

void RenderWorld::freeLightWithID(int index) {
	setLight(index, vec3(0), vec3(0));
	//printf("Light %i freed\n", index);
	lightsInUse[index] = false;
}


void RenderWorld::addInstance(Model &model, int id, mat4 transform) {
	bool createNewModelGroup = true;
	Instance instance = { id,transform };
	for (int i = 0; i < sceneModels.size(); ++i) {
		if (sceneModels[i].meshes[0].VAO == model.modelData.meshes[0].VAO) {
			sceneModels[i].instances.push_back(instance);
			createNewModelGroup = false;
		}
	}
	if (createNewModelGroup) {
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

void RenderWorld::updateInstance(Model &model, int id, mat4 transform) {
	for (int i = 0; i < sceneModels.size(); ++i) {
		if (sceneModels[i].meshes[0].VAO == model.modelData.meshes[0].VAO) {
			for (int j = 0; j < sceneModels[i].instances.size(); ++j) {
				if (sceneModels[i].instances[j].ID == id) {
					sceneModels[i].instances[j].transform = transform;
				}
			}
		}
	}
}

void RenderWorld::removeInstance(Model &model, int id) {
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

void RenderWorld::passLights(GLuint shaderID) {
	for (int i = 0; i < lights.size(); i++) {
		std::string lightsPos = "lights[" + std::to_string(i) + "].position";
		std::string lightsColor = "lights[" + std::to_string(i) + "].color";
		ShaderUniforms::setVec3(shaderID, lightsPos, lights[i].position);
		ShaderUniforms::setVec3(shaderID, lightsColor, lights[i].color);
	}
}

void RenderWorld::passCamera(GLuint shaderID) {
	ShaderUniforms::setMat4(shaderID, "view", cameraDetails.view);
	ShaderUniforms::setMat4(shaderID, "projection", cameraDetails.projection);
	ShaderUniforms::setVec3(shaderID, "viewPos", cameraDetails.location);
}

void RenderWorld::passTextures(GLuint shaderID) {
	// setup texture shaders
	ShaderUniforms::setInt(shaderID, "textureData.albedo", 0);
	ShaderUniforms::setInt(shaderID, "textureData.roughness", 1);
	ShaderUniforms::setInt(shaderID, "textureData.metalness", 2);
	ShaderUniforms::setInt(shaderID, "textureData.normal", 3);
	ShaderUniforms::setInt(shaderID, "textureData.emission", 4);
}

void RenderWorld::activateTextures(RendererModel sModel) {
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

//TODO fix this method, shaders are probably broken
void RenderWorld::loadCubemap() {
	glGenTextures(1, &cubemap.textureID);

	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap.textureID);

	std::vector<std::string> names; 
	names = {
		"stars_right1.png",
		"stars_left2.png",
		"stars_top3.png",
		"stars_bottom4.png",
		"stars_front5.png",
		"stars_back6.png" };
	std::string folder = "cubemap";
	cubemap.textureID = TextureDataManager::getCubemapData(folder, names)->textureID;

	cubemap.shaderID = ShaderDataManager::getShaderData("cubemap")->shaderID;

	ModelData *tempModelData = ModelDataManager::getModelData("cube.obj");
	
	cubemap.model.VAO = tempModelData->meshes[0].VAO;
	cubemap.model.trisCount = tempModelData->meshes[0].indices.size();
}