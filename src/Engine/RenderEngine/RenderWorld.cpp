#include "RenderWorld.h"
#include "ShaderUniforms.h"
#include "../Importer/Managers/TextureDataManager.h"
#include "../Importer/Managers/ShaderDataManager.h"
#include "../Importer/Managers/ModelDataManager.h"

using namespace glm;

RenderWorld::RenderWorld(int *screenWidth, int *screenHeight) {
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;
	for (int i = 0; i < NUM_LIGHTS; ++i) {
		Light newLight;
		newLight.position = vec3(0);
		newLight.color = vec3(0);
		lights.push_back(newLight);
		lightsInUse[i] = false;
	}
	setLight(0, vec3(0, 100.0, 0), vec3(20000));
//	setLight(0, vec3(100.0, 100.0, 100.0), vec3(2000.0, 2000.0, 2000.0));
//	setLight(1, vec3(100.0, 100.0, -100.0), vec3(20000.0));
//	setLight(2, vec3(-100.0, 100.0, 100.0), vec3(20000.0));
//	setLight(3, vec3(-100.0, 100.0, -100.0), vec3(20000.0));
//	lightsInUse[0] = lightsInUse[1] = lightsInUse[2] = lightsInUse[3] = true;
	lightsInUse[0] = true;

	//load the skybox
	PrepShadowMaps();
	loadSkybox();
//	for (int i = 0; i < shadows.maps.size(); ++i) {
//		std::cout << shadows.maps[i] << std::endl;
//	}
}

RenderWorld::~RenderWorld() {

}

void RenderWorld::renderScene() {
	glViewport(0, 0, shadows.SIZE, shadows.SIZE);
	genShadows();
	glViewport(0, 0, *screenWidth, *screenHeight);
	renderCubemap();
	renderCamera();
}

void RenderWorld::PrepShadowMaps() {
	//GLint texture_units;
	//glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &texture_units);
	//std::cout << texture_units << std::endl;

	//Create shadow shader
	shadows.shaderID = ShaderDataManager::getShaderData("pointShadows")->shaderID;

	//Create framebuffer for shadows
	glGenFramebuffers(1, &shadows.depthFBO);

	//Create cubemaps for shadow maps
	for (int i = 0; i < NUM_SHADOWS; ++i) {
		shadows.maps.push_back(0);
//		std::cout << shadows.maps[i] << std::endl;
		glGenTextures(1, &shadows.maps[i]);
		glBindTexture(GL_TEXTURE_CUBE_MAP, shadows.maps[i]);
		for (int j = 0; j < 6; ++j) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + j, 0, GL_DEPTH_COMPONENT, shadows.SIZE, shadows.SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		//std::cout << shadows.maps[i] << std::endl;
	}
}

void RenderWorld::genShadows() {
	for (int i = 0; i < shadows.maps.size(); ++i) {
		// attach depth texture as FBO's depth buffer
		glBindFramebuffer(GL_FRAMEBUFFER, shadows.depthFBO);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadows.maps[i], 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		// create depth cubemap tranformation matrices
		glm::vec3 lightPos = lights[i].position;
		float nearPlane = 1.0f;
		glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)shadows.SIZE / shadows.SIZE, nearPlane, farPlane);
		std::vector<glm::mat4> shadowTransforms;
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

		// render scene to depth cubemap
		glClear(GL_DEPTH_BUFFER_BIT);
		glUseProgram(shadows.shaderID);
		for (unsigned int i = 0; i < 6; ++i) {
			ShaderUniforms::setMat4(shadows.shaderID, "shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
		}

		ShaderUniforms::setFloat(shadows.shaderID, "farPlane", farPlane);
		ShaderUniforms::setVec3(shadows.shaderID, "lightPos", lightPos);

		for (RendererModel &sModel : sceneModels) {
			renderElements(sModel, shadows.shaderID);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

void RenderWorld::renderElements(RendererModel &sModel, GLuint shaderID) {

	GLint transformationLoc = glGetUniformLocation(shaderID, "modelTransformation");

	for (auto &mesh : sModel.meshes) {
		for (int j = 0; j < sModel.instances.size(); ++j) {
			glBindVertexArray(mesh.VAO);
			glUniformMatrix4fv(transformationLoc, 1, GL_FALSE, value_ptr(sModel.instances[j].transform));
			glDrawElements(GL_TRIANGLES, mesh.trisCount, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}
	}
}

void RenderWorld::renderCamera() {
	GLuint currentShaderID = -1;
	for (RendererModel &sModel : sceneModels) {

		if (sModel.shaderID != currentShaderID) {
			currentShaderID = sModel.shaderID;
			glUseProgram(currentShaderID);

			passLights(currentShaderID); //pass lights into shader
			passCamera(currentShaderID); //pass camera into shader
			passTextures(currentShaderID); //pass textures into shader
			ShaderUniforms::setFloat(currentShaderID, "farPlane", farPlane);
		}

		activateTextures(sModel);
		renderElements(sModel, currentShaderID);
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
//	glBindTexture(GL_TEXTURE_CUBE_MAP, shadows.maps[0]);
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
	for (int i = 0; i < shadows.maps.size(); ++i) {
		ShaderUniforms::setInt(shaderID, "shadowData[" + std::to_string(i) + "]", 5 + i);
	}
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
	for (int i = 0; i < shadows.maps.size(); ++i) {
		glActiveTexture(GL_TEXTURE5 + i);
		glBindTexture(GL_TEXTURE_CUBE_MAP, shadows.maps[i]);
	}
}

void RenderWorld::loadSkybox() {
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