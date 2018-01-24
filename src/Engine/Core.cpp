//
// Created by Sebastian on 12/23/2017.
//

#include "Core.h"
#include "../Game/Logic/Logic.h"
#include "../Art/MeshData.h"
#include "../Art/ShaderData.h"
#include "../Game/Camera.h"
#include "PhysicsEngine\PhysicsEngine.h"
#include "../Game/Components/RendererComponent.h"
#include "../Engine/EntityManager.h"
#include "../Engine/ComponentManager.h"
#include "../Engine/Entity.h"

Core::Core(int screenWidth,int screenHeight, GLFWwindow *window, bool gamePaused) {
    //this->properties.openGL_Program = openGL_Program;
    this->properties.window = window;
    this->properties.screenWidth = screenWidth;
    this->properties.screenHeight = screenHeight;
    this->properties.isRunning = true;
    this->properties.isPaused = gamePaused;
}

Core::~Core() = default;

// uhhh should these be in core? Can change/delete as needed
// keyboard for yaw and pitch
vec2 cameraRoll(GLFWwindow *window) {
	float xpos = 0;
	float ypos = 0;
	float mov = 0.01f;

	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		xpos -= mov;
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		xpos += mov;
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		ypos += mov;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		ypos -= mov;

	return vec2(xpos, ypos);
}

// keyboard movement for WASD
void processCameraInput(GLFWwindow *window, Movement &movement) {
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		movement.forward = true;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		movement.backward = true;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		movement.left = true;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		movement.right = true;
}


void resetMovement(Movement &movement) {
	movement.right = movement.left = movement.forward = movement.backward = false;
}

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
    tempMesh.loadMeshData("data/assets/meshes/cube.obj");
    MeshData planeMesh("planeMesh");
    planeMesh.loadMeshData("data/assets/meshes/plane.obj");

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

    physx::PxVehicleDrive4W* myVehicle = physicsEngine.createVehicle();
    //printf("Number of shapes: %i", myVehicle->getRigidDynamicActor()->getNbShapes());

	// New Entity creation code, place at center of screen, no rotation, scale of 1.
	auto entity1 = EntityManager::getInstance()->createEntity(glm::vec3(0.f), glm::quat(), glm::vec3(1.f));
    ComponentManager::getInstance()->addRendererComponent(entity1, &tempMesh, &shaderData, glm::vec3(0,0,0),glm::quat(glm::vec3(0, -1.57, 0)),glm::vec3(2));
    ComponentManager::getInstance()->addPhysicsComponent(entity1, myVehicle->getRigidDynamicActor());
    
    auto entity2 = EntityManager::getInstance()->createEntity(glm::vec3(0.f), glm::quat(), glm::vec3(1.f));
    ComponentManager::getInstance()->addRendererComponent(entity2, &planeMesh, &shaderData, glm::vec3(0, 0, 0), glm::quat(glm::vec3(0, 0, 0)), glm::vec3(10,10,100));
    
    ComponentManager::getInstance()->initializeRendering(&renderEngine);
    // -----------------End of temp initialize model/instance in rendering code

	double previousTime = 0;
    float physicsTime = 0;
    const float physicsTimeStep = 1.0f / 60.0f;

    // Used to move the camera alongside the cube vehicle
	// Now used such that WASD is used to move camera
    Movement movement;
    //movement.right = false;
    //float previousZ = 0;


	
	
	

	while (properties.isRunning && !glfwWindowShouldClose(properties.window)){
        glfwPollEvents();

		const auto currentTime = glfwGetTime();
	    const auto timeDiff = currentTime - previousTime;
		previousTime = currentTime;
        

        //-----Temp rotation code:
        //Setup a time based rotation transform to demo that updateInstance works
        /*mat4 transform00;
        transform00 = glm::rotate(transform00,GLfloat(timeDiff) * 5.0f,vec3(0,0,1));
        renderEngine.updateInstance(tempMesh,0,transform00);*/

        //------End of temp rotation code

        //We could make a pause game feature by just rendering stuff and disabling all
        // the other stuff... although feel free to change this if you think some other
        // approach is better
        if(properties.isPaused){
            renderEngine.render(camera);
        }else{
            
            // Simulate physics in a Fixed Timestep style
            while(physicsTime < currentTime) {
                physicsTime += physicsTimeStep;
                physicsEngine.simulateTimeInSeconds(physicsTimeStep, myVehicle);
                //const physx::PxVec3 pos = myVehicle->getRigidDynamicActor()->getGlobalPose().p;
                //printf("Vehicle position: (%f, %f, %f)\n", pos.x, pos.y, pos.z);
            }
            // Move the camera to track the vehicle
			
            const physx::PxVec3 pos = myVehicle->getRigidDynamicActor()->getGlobalPose().p;
            //camera.moveCamera(movement, pos.z - previousZ);
            //previousZ = pos.z;
            
			camera.lookAtPos = glm::vec3(pos.x, pos.y, pos.z);
			processCameraInput(this->properties.window, movement);
			camera.moveCamera(movement, 0.5f); // just some number for the speed...
			resetMovement(movement);
			camera.rotateView(cameraRoll(this->properties.window));
			
            // Render all of the renderer components here
            ComponentManager::getInstance()->performPhysicsLogic();
            ComponentManager::getInstance()->performRendering(&renderEngine);

            renderEngine.render(camera);
            audioEngine.simulate();
        }
    }
}
