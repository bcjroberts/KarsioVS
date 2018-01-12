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
    glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderElements(camera);


    glUseProgram(0); //cleanup

    // Swap the screen buffers
    glfwSwapBuffers(window);
}


void RenderEngine::renderElements(Camera camera) {
    ShaderData currentShader;
    GLint transformationLoc;
    for (auto &sModel : sceneModels) {

        //std::cout<<&sModel<<std::endl;
        //std::cout<<&sModel.materialData<<std::endl;
        //std::cout<<&sModel.meshData.meshDataPointer<<std::endl;
        //std::cout<<&sModel.meshData.vertexIndices<<std::endl;
        //std::cout<<&sModel.meshData.uvIndices<<std::endl;
        //std::cout<<&sModel.meshData.normalIndices<<std::endl;
        //std::cout<<&sModel.meshData.verticies<<std::endl;
        //for (int i = 0; i < sModel.meshData.verticies.size(); ++i) {
        //    std::cout<<&sModel.meshData.verticies[i].x<<",";
        //    std::cout<<&sModel.meshData.verticies[i].y<<",";
        //    std::cout<<&sModel.meshData.verticies[i].z<<std::endl;
        //}

        //RenderData model = sceneModels[i].model;
        //mShaders.use();

        //if(sModel.shaderData.id!=currentShader.id){
        //
        //}
        currentShader = sModel.shaderData;
        currentShader.use();
        // Camera/View transformation
        GLint viewLoc = glGetUniformLocation(currentShader.id, "view");
        GLint projLoc = glGetUniformLocation(currentShader.id, "projection");
        GLint viewPosLoc = glGetUniformLocation(currentShader.id, "viewPos");
        transformationLoc = glGetUniformLocation(currentShader.id, "modelTransformation");
        camera.setupCameraTransformationMatrices(viewLoc,projLoc,viewPosLoc);

        //Geometry geometry;
        //setupBuffers(sModel);
        for (int j = 0; j < sModel.instances.size(); ++j) {
            sModel.geometry.bindBuffer();
            glUniformMatrix4fv(transformationLoc, 1, GL_FALSE, value_ptr(sModel.instances[j].transform));
            glDrawArrays(GL_TRIANGLES, 0, sModel.geometry.triangleCount);
            glBindVertexArray(0);
        }
    }
    //std::cout<<"rendering"<<std::endl;
}

void RenderEngine::addInstance(MeshData &model, int id, mat4 transform, ShaderData &shaderData) {
    bool createNewModelGroup = true;
    Instance instance = {id,transform};
    for (int i = 0; i < sceneModels.size(); ++i) {
        if(sceneModels[i].geometry.meshDataPointer==&model){
            sceneModels[i].instances.push_back(instance);
            createNewModelGroup = false;
        }
    }
    if(createNewModelGroup){
        Models models; //deep copy values from main model data to render specific model list
        models.shaderData = shaderData;
        models.geometry.makeBuffer(model);

        //models.meshData = model.mesh;
        //models.shaderData = shaderData;
        //models.meshData.meshDataPointer = &model;
        //
        //models.meshData.geometry;
        //
        //models.meshData.vertexIndices = model.mesh.vertexIndices;
        //models.meshData.normalIndices = model.mesh.normalIndices;
        //models.meshData.uvIndices = model.mesh.uvIndices;
        //
        //models.meshData.verticies = model.mesh.verticies;
        //models.meshData.normals = model.mesh.normals;
        //models.meshData.uvs = model.mesh.uvs;
        //models.model = model;

        models.instances.push_back(instance);

        sceneModels.push_back(models);
    }
}

void RenderEngine::updateInstance(MeshData &model, int id, mat4 transform) {
    for (int i = 0; i < sceneModels.size(); ++i) {
        if(sceneModels[i].geometry.meshDataPointer==&model){
            for (int j = 0; j < sceneModels[i].instances.size(); ++j) {
                if(sceneModels[i].instances[j].ID==id){
                    sceneModels[i].instances[j].transform = transform;
                }
            }
        }
    }
}
