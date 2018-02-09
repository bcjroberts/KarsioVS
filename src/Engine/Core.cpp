//
// Created by Sebastian on 12/23/2017.
//

#include "Core.h"
#include "../Game/Logic/Logic.h"
#include "Importer/Data/ModelData.h"
#include "Importer/Data/ShaderData.h"
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
int cameraMode = 0;

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

	// Controls whether the camera is free or locked to the player vehicle
	if (key == GLFW_KEY_1) {
		cameraMode = 1;
	} else if (key == GLFW_KEY_2) {
		cameraMode = 0;
	}
}


//Main game loop
void Core::coreLoop() {
    RenderEngine renderEngine(properties.window);
    AudioEngine audioEngine;
    Logic logic;

	glfwSetKeyCallback(properties.window, windowKeyInput);
	glfwSetInputMode(properties.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);


    PhysicsEngine::getInstance()->initPhysics();

    //initialize camera class, in theory if you modify the position/direction of
    // the camera class it will affect the rendered view
    Camera camera(properties.screenWidth,properties.screenHeight);

    //following set of functions opens an object file, I considered sticking an shaderID
    // of some kind into the mesh data (hence the constructor takes a string) but
    // in actually that's never used
//    ModelData cubeMesh("cubeMesh");
//    cubeMesh.loadMeshData("data/assets/meshes/cube.obj");

    //Following set of functions adds the shaders to the shader class and then links them
//    ShaderData shaderData;
//    shaderData.attachShader("data/shaderData/vertex.glsl",GL_VERTEX_SHADER);
//    shaderData.attachShader("data/shaderData/fragment.glsl",GL_FRAGMENT_SHADER);
//    shaderData.link();


    // Brian's shenanigans go here
    // Don't mind me, just trying to figure out how to shove everything into logic
    logic.bindCamera(&camera);
    // END Brian's shenanigans

	// Perform some minor world generation.


    // Create the starting Entities
    Entity* entity1 = EntityManager::getInstance()->createBasicVehicleEntity(glm::vec3(0, 1, 0));
	Entity* entity5 = EntityManager::getInstance()->createBasicVehicleEntity(glm::vec3(10, 1, 0));
	Entity* entity2 = EntityManager::getInstance()->createGroundPlane();
	Entity* boxEntity = EntityManager::getInstance()->createBox(glm::vec3(5,1.0f,5),glm::vec3(1,1,1));

    ComponentManager::getInstance()->initializeRendering(&renderEngine);
    // -----------------End of temp initialize model/instance in rendering code

	double previousTime = 0;
    float physicsTime = 0;
    const float physicsTimeStep = 1.0f / 60.0f;
	
	// for yaw/pitch controlled by cursor
	double xpos, ypos;
    bool movingForward = true;

	while (properties.isRunning && !glfwWindowShouldClose(properties.window)){
        glfwPollEvents();

		const auto currentTime = glfwGetTime();
	    const auto timeDiff = currentTime - previousTime;
		previousTime = currentTime;
        
		// should move these to AI system, but right now this is here
		AStar::Generator generator;
		generator.setWorldSize({ 10, 10 });
		logic.findPath(&generator, entity1, boxEntity);

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
            
//			printf("FrameTime: %f", timeDiff);

            // Simulate physics in a Fixed Timestep style
            while(physicsTime < currentTime) {
//				printf(" *");
                physicsTime += physicsTimeStep;
                PhysicsEngine::getInstance()->simulateTimeInSeconds(physicsTimeStep);
            }
//			printf("\n");

			logic.playerMovement(&tempPlayerInput, entity1);

            // Render all of the renderer components here
            ComponentManager::getInstance()->performPhysicsLogic();
            ComponentManager::getInstance()->performRendering(&renderEngine);

            if (cameraMode == 0)
            {
                // Move camera by keyboard and cursor
                glfwGetCursorPos(properties.window, &xpos, &ypos);
                camera.rotateView(vec2(xpos / properties.screenWidth, -ypos / properties.screenHeight));
                camera.moveCamera(movement, timeDiff * 50.0f);
            }
            else if (cameraMode == 1)
            {
                // Force the camera to follow the vehicle
                // get the velocity of the vehicle
                glm::vec3 velocity = PhysicsEngine::toglmVec3(static_cast<PhysicsComponent*>(entity1->getComponent(PHYSICS))->getRigidBody()->getLinearVelocity());
                glm::vec3 offset = entity1->getForwardVector();

                float dotProd = glm::dot(velocity, entity1->getForwardVector());

                if (dotProd < -2.5f && tempPlayerInput.brake) {
                    movingForward = false;
                } else if (dotProd > -2.0f && tempPlayerInput.accel) {
                    movingForward = true;
                }

                if (!movingForward) offset = -offset;

                camera.rotateCameraTowardPoint(entity1->getPosition() + offset * 10.0f, 10.0f * timeDiff);
                camera.lerpCameraTowardPoint(entity1->getPosition() + offset * -8.0f + glm::vec3(0, 8, 0), 5.0f * timeDiff);
            }

            renderEngine.render(camera);
            audioEngine.simulate();
        }
    }
}
