//
// Created by Sebastian on 12/23/2017.
//

#include "Core.h"
#include "../Game/Logic/Logic.h"
#include "../Art/MeshData.h"
#include "../Art/ShaderData.h"
#include "../Game/Camera.h"
#include "PhysicsEngine\PhysicsEngine.h"
#include "../Engine/EntityManager.h"
#include "../Engine/ComponentManager.h"
#include "../Engine/Entity.h"
//Used for my not-so-great struct -Brian
#include "../Game/Components/DriveComponent.h"

Core::Core(int screenWidth,int screenHeight, GLFWwindow *window, bool gamePaused) {
    //this->properties.openGL_Program = openGL_Program;
    this->properties.window = window;
    this->properties.screenWidth = screenWidth;
    this->properties.screenHeight = screenHeight;
    this->properties.isRunning = true;
    this->properties.isPaused = gamePaused;
}

Core::~Core() = default;

vehicleInput tempPlayerInput;

// there has to be a better way than to make it this way
Movement movement;

// camera, using keyboard events for WASD
void windowKeyInput(GLFWwindow *window, int key, int scancode, int action, int mods) {
	bool set = action != GLFW_RELEASE && GLFW_REPEAT;
	switch (key) {
	case GLFW_KEY_ESCAPE:
		glfwSetWindowShouldClose(window, GL_TRUE);
		break;
	case GLFW_KEY_W:
		movement.forward = set ? 1 : 0;
		break;
	case GLFW_KEY_A:
		movement.left = set ? 1 : 0;
		break;
	case GLFW_KEY_S:
		movement.backward = set ? 1 : 0;
		break;
	case GLFW_KEY_D:
		movement.right = set ? 1 : 0;
		break;
	case GLFW_KEY_SPACE:
		movement.up = set ? 1 : 0;
		break;
	case GLFW_KEY_C:
		movement.down = set ? 1 : 0;
		break;

    // Vehicle movement, adding to the pile of broken stuff

    case GLFW_KEY_LEFT:
        tempPlayerInput.steerRight = set ? 1 : 0;
        break;
    case GLFW_KEY_RIGHT:
        tempPlayerInput.steerLeft = set ? 1 : 0;
        break;
    case GLFW_KEY_UP:
        tempPlayerInput.accel = set ? 1 : 0;
        break;
    case GLFW_KEY_DOWN:
        tempPlayerInput.brake = set ? 1 : 0;
        break;
	}
}


//Main game loop
void Core::coreLoop() {
    RenderEngine renderEngine(properties.window);
    AudioEngine audioEngine;
    Logic logic;

	glfwSetKeyCallback(properties.window, windowKeyInput);
	glfwSetInputMode(properties.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    PhysicsEngine::getInstance()->initPhysics();
    // -----------Temp code, to initialize model/instance in rendering code...
    // Obviously this should be moved elsewhere when it's being used for real...

    //initialize camera class, in theory if you modify the position/direction of
    // the camera class it will affect the rendered view
    Camera camera(properties.screenWidth,properties.screenHeight);

    //following set of functions opens an object file, I considered sticking an id
    // of some kind into the mesh data (hence the constructor takes a string) but
    // in actually that's never used
    MeshData cubeMesh("cubeMesh");
    cubeMesh.loadMeshData("data/assets/meshes/cube.obj");
    MeshData wheelMesh("wheelMesh");
    wheelMesh.loadMeshData("data/assets/meshes/wheels.obj");
    MeshData chassisMesh("chassisMesh");
    chassisMesh.loadMeshData("data/assets/meshes/chassis.obj");
    MeshData planeMesh("planeMesh");
    planeMesh.loadMeshData("data/assets/meshes/plane.obj");

    //Following set of functions adds the shaders to the shader class and then links them
    ShaderData shaderData;
    shaderData.attachShader("data/shaderData/vertex.glsl",GL_VERTEX_SHADER);
    shaderData.attachShader("data/shaderData/fragment.glsl",GL_FRAGMENT_SHADER);
    shaderData.link();


    // Brian's shenanigans go here
    // Don't mind me, just trying to figure out how to shove everything into logic
    logic.bindCamera(&camera);
    // END Brian's shenanigans

    ////////// NEW way to create vehicles. TODO: Should not have to pass in the shader data!
    Entity* entity1 = EntityManager::getInstance()->createBasicVehicleEntity(&shaderData);

    auto entity2 = EntityManager::getInstance()->createEntity(glm::vec3(0.f), glm::quat(), glm::vec3(1.f));
    ComponentManager::getInstance()->addRendererComponent(entity2, &planeMesh, &shaderData, glm::vec3(0, 0, 0), glm::quat(glm::vec3(0, 0, 0)), glm::vec3(10,10,100));
    
    ComponentManager::getInstance()->initializeRendering(&renderEngine);
    // -----------------End of temp initialize model/instance in rendering code

	double previousTime = 0;
    float physicsTime = 0;
    const float physicsTimeStep = 1.0f / 60.0f;

    // Used to move the camera alongside the cube vehicle
	// Now used such that WASD is used to move camera
    //Movement movement;
    //movement.right = false;
    //float previousZ = 0;
	
	// for yaw/pitch controlled by cursor
	double xpos, ypos;
	

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
                PhysicsEngine::getInstance()->simulateTimeInSeconds(physicsTimeStep);
            }
			
            // set the movement of the vehicle
            static_cast<DriveComponent*>(entity1->getComponent(DRIVE))->setInputs(tempPlayerInput);
            
			// Move camera by keyboard and cursor
			glfwGetCursorPos(properties.window, &xpos, &ypos);
			camera.rotateView(vec2(xpos / properties.screenWidth, -ypos / properties.screenHeight));
			camera.moveCamera(movement, 0.5f); // just some number for the time delta...
            logic.cameraMovement(&movement);
            logic.playerMovement(&tempPlayerInput, entity1);

            // Render all of the renderer components here
            ComponentManager::getInstance()->performPhysicsLogic();
            ComponentManager::getInstance()->performRendering(&renderEngine);

            renderEngine.render(camera);
            audioEngine.simulate();
        }
    }
}
