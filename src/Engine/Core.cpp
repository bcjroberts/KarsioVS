//
// Created by Sebastian on 12/23/2017.
//

#include "Core.h"
#include "../Game/Logic/Logic.h"
#include "../Art/MeshData.h"
#include "../Art/ShaderData.h"
#include "../Game/Camera.h"
#include "PhysicsEngine\PhysicsEngine.h"
#include <time.h>

//#include "PxPhysicsAPI.h"

Core::Core(int screenWidth,int screenHeight, GLFWwindow *window, bool gamePaused) {
    //this->properties.openGL_Program = openGL_Program;
    this->properties.window = window;
    this->properties.screenWidth = screenWidth;
    this->properties.screenHeight = screenHeight;
    this->properties.isRunning = true;
    this->properties.isPaused = gamePaused;
}

Core::~Core() {

}

/*physx::PxDefaultErrorCallback gDefaultErrorCallback;
physx::PxDefaultAllocator gDefaultAllocatorCallback;
physx::PxFoundation* mFoundation = NULL;*/

//Main game loop
void Core::coreLoop() {
    RenderEngine renderEngine(properties.window);
    PhysicsEngine physicsEngine;
    AudioEngine audioEngine;
    Logic logic;

    physicsEngine.initPhysics();
    // -----------Temp code, to initialize model/instance in rendering code...
    // Obviously this should be moved elsewhere when it's being used for real...

    //initialize camera class, in theory if you modify the position/direction of
    // the camera class it will affect the rendered view
    Camera camera(properties.screenWidth,properties.screenHeight);

    //following set of functions opens an object file, I considered sticking an id
    // of some kind into the mesh data (hence the constructor takes a string) but
    // in actually that's never used
    MeshData tempMesh("testObject1");
    tempMesh.loadMeshData("data/assets/meshes/tempCar.obj");

    //Following set of functions adds the shaders to the shader class and then links them
    ShaderData shaderData;
    shaderData.attachShader("data/shaderData/vertex.glsl",GL_VERTEX_SHADER);
    shaderData.attachShader("data/shaderData/fragment.glsl",GL_FRAGMENT_SHADER);
    shaderData.link();

    //Sets up the first instance of the temp mesh that has been created above and
    // tells the render engine that it exists
    /// In theory this will be modified later to already have a mesh list setup so all
    /// you'd need to do is tell it that a certain mesh needs an instance.
    /// Currently the mesh is created if it doesn't exist yet but once it exists the
    /// instance is added to that mesh using the address of the mesh.
    mat4 transform0;
    renderEngine.addInstance(tempMesh,0,transform0,shaderData); //Draw object at 0,0,0

    //sets up the second instance of the temp mesh that has been created above and
    // translates it up 2 units before telling the render engine that it exists
    mat4 transform1;
    transform1 = glm::translate(transform1,vec3(0,2,0));
    renderEngine.addInstance(tempMesh,1,transform1,shaderData); //draw object at 0,2,0

    //sets up the third instance of the temp mesh that has been created above and
    // translates it down 2 units before telling the render engine that it exists
    mat4 transform2;
    transform2 = glm::translate(transform2, vec3(0,-2,0));
    renderEngine.addInstance(tempMesh,2,transform2,shaderData); //draw object at 0,-2,0

	/*mFoundation = PxCreateFoundation(PX_FOUNDATION_VERSION, gDefaultAllocatorCallback,
		gDefaultErrorCallback);*/

    // -----------------End of temp initialize model/instance in rendering code
    clock_t previousTime = clock();

    while (properties.isRunning && !glfwWindowShouldClose(properties.window)){
        glfwPollEvents();

        //-----Temp rotation code:
        //Setup a time based rotation transform to demo that updateInstance works
        mat4 transform00;
        transform00 = glm::rotate(transform00,(GLfloat)glfwGetTime() * 5.0f,vec3(0,0,1));
        renderEngine.updateInstance(tempMesh,0,transform00);

        //------End of temp rotation code

        //We could make a pause game feature by just rendering stuff and disabling all
        // the other stuff... although feel free to change this if you think some other
        // approach is better
        clock_t nowTime = clock();
        int ms = double(nowTime - previousTime) / CLOCKS_PER_SEC * 1000;
        printf("%i time diff\n", ms);
        previousTime = nowTime;
        if(properties.isPaused){
            renderEngine.render(camera);
        }else{
            physicsEngine.simulateTimeInSeconds(float(ms)/1000.0f);
            renderEngine.render(camera);
            audioEngine.simulate();
        }
    }
}