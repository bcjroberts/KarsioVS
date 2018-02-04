//
// Created by Sebastian on 12/23/2017.
//

#include <iostream>
#include "RenderEngine.h"

RenderEngine::RenderEngine(GLFWwindow *window) {
    this->window = window;
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


void RenderEngine::renderElements(Camera camera) {
//    ShaderData currentShader;
    GLint transformationLoc;
    for (auto &sModel : sceneModels) {

//        std::cout<<&sModel<<std::endl;
//        std::cout<<&sModel.materialData<<std::endl;
//        std::cout<<&sModel.meshData.meshDataPointer<<std::endl;
//        std::cout<<&sModel.meshData.vertexIndices<<std::endl;
//        std::cout<<&sModel.meshData.uvIndices<<std::endl;
//        std::cout<<&sModel.meshData.normalIndices<<std::endl;
//        std::cout<<&sModel.meshData.vertices<<std::endl;
//        for (int i = 0; i < sModel.meshData.vertices.size(); ++i) {
//            std::cout<<&sModel.meshData.vertices[i].x<<",";
//            std::cout<<&sModel.meshData.vertices[i].y<<",";
//            std::cout<<&sModel.meshData.vertices[i].z<<std::endl;
//        }

        //RenderData model = sceneModels[i].model;
        //mShaders.use();

        //if(sModel.shaderData.shaderID!=currentShader.shaderID){
        //
        //}
	    glUseProgram(sModel.shaderID);

//        currentShader = sModel.shaderData;
		
//        currentShader.use();
        // Camera/View transformation
        GLint viewLoc = glGetUniformLocation(sModel.shaderID, "view");
        GLint projLoc = glGetUniformLocation(sModel.shaderID, "projection");
        GLint viewPosLoc = glGetUniformLocation(sModel.shaderID, "viewPos");
        transformationLoc = glGetUniformLocation(sModel.shaderID, "modelTransformation");
        camera.setupCameraTransformationMatrices(viewLoc,projLoc,viewPosLoc);

//		std::cout <<"ID: "<< sModel.shaderID << std::endl;
//		std::cout <<"Count: "<< sModel.meshes[0].trisCount << std::endl;
//		std::cout <<"VAO: "<< sModel.meshes[0].VAO << std::endl;
        //Geometry geometry;
        //setupBuffers(sModel);
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

void RenderEngine::addInstance(ModelData &model, int id, mat4 transform) {
    bool createNewModelGroup = true;
    Instance instance = {id,transform};
    for (int i = 0; i < sceneModels.size(); ++i) {
        if(sceneModels[i].meshes[0].VAO == model.meshes[0].VAO){
            sceneModels[i].instances.push_back(instance);
            createNewModelGroup = false;
        }
    }
    if(createNewModelGroup){
		RendererModel rendererModel; //deep copy values from main model data to render specific model list
		rendererModel.shaderID = model.materialData.shaderData.shaderID;
//		model.geometry.makeBuffer(model);
		for (int i = 0; i < model.meshes.size(); i++) {
			Geometry mesh;
			mesh.VAO = model.meshes[i].VAO;
			mesh.trisCount = model.meshes[i].indices.size();
			rendererModel.meshes.push_back(mesh);
		}
		rendererModel.instances.push_back(instance);

        sceneModels.push_back(rendererModel);
    }
}

void RenderEngine::updateInstance(ModelData &model, int id, mat4 transform) {
    for (int i = 0; i < sceneModels.size(); ++i) {
        if (sceneModels[i].meshes[0].VAO == model.meshes[0].VAO) {
            for (int j = 0; j < sceneModels[i].instances.size(); ++j) {
                if(sceneModels[i].instances[j].ID==id){
                    sceneModels[i].instances[j].transform = transform;
                }
            }
        }
    }
}
