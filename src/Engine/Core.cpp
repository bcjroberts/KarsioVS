//
// Created by Sebastian on 12/23/2017.
//

#include "Core.h"
#include "../Game/Logic/Logic.h"
#include "Importer/Data/Model.h"
#include "Importer/Data/ShaderData.h"
#include "../Game/Camera.h"
#include "PhysicsEngine\PhysicsEngine.h"
#include "../Engine/EntityManager.h"
#include "../Engine/ComponentManager.h"
#include "../Engine/Entity.h"
//Used for my not-so-great struct -Brian
#include "../Game/Components/DriveComponent.h"
#include "../Game/Logic/WorldGenerator.h"

GLFWwindow* Core::globalWindow = nullptr;

Core::Core(int *screenWidth,int *screenHeight, GLFWwindow *window, bool gamePaused) {
    //this->properties.openGL_Program = openGL_Program;
    this->properties.window = window;
    this->properties.screenWidth = screenWidth;
    this->properties.screenHeight = screenHeight;
    this->properties.isRunning = true;
    this->properties.isPaused = gamePaused;
    globalWindow = properties.window;
}

Core::~Core() = default;

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
//    Model cubeMesh("cubeMesh");
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

    // Create the starting Entities
    Entity* playerVehicle = EntityManager::getInstance()->createPlayerVehicleEntity(glm::vec3(0, 1, 0));
	Entity* aiVehicle = EntityManager::getInstance()->createAIVehicleEntity(glm::vec3(10, 1, 0));
	Entity* groundPlane = EntityManager::getInstance()->createGroundPlane();

	WorldGenerator worldGen;
	worldGen.generateWorld();
	
	AStar::Generator gen;
	gen.setWorldSize({ 20, 20 });

	// TODO: world size doesn't match actual size

	std::vector<Entity*>* obstacles = worldGen.getObstacles();
	std::vector<Entity*>* crystals = worldGen.getCrystals();
	float x, y;
	for (int i = 0; i < obstacles->size() - 1; i++) {
		x = (*obstacles)[i]->getCoarsePosition().x;
		y = (*obstacles)[i]->getCoarsePosition().z;
		gen.addCollision(vec2(x, y));
	}
	for (int i = 0; i < crystals->size() - 1; i++) {
		x = (*crystals)[i]->getCoarsePosition().x;
		y = (*crystals)[i]->getCoarsePosition().z;
		gen.addCrystal(vec2(x, y));
	}
	//logic.findPath(&gen, aiVehicle->getPosition(), glm::vec3(-50, 1.0f, -50));
	
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
	    float timeDiff = currentTime - previousTime;
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
            
//			printf("FrameTime: %f", timeDiff);
			timeDiff = 0.0f;
			// Simulate physics in a Fixed Timestep style
			while (physicsTime < currentTime) {
				//				printf(" *");
				physicsTime += physicsTimeStep;
				PhysicsEngine::getInstance()->simulateTimeInSeconds(physicsTimeStep);
				timeDiff += 1.0f / 60.0f;
			}
//			printf("\n");
			//std::cout << "current position " << playerVehicle->getCoarsePosition().x << " " << playerVehicle->getCoarsePosition().z << std::endl;
			//std::cout << "current position " << aiVehicle->getPosition().x << " " << aiVehicle->getPosition().z << std::endl;

			//logic.playerMovement(&tempPlayerInput, playerVehicle);
			logic.playerMovement(playerVehicle);
            //logic.aiMovement(aiVehicle);
			logic.stateThing(aiVehicle, &gen, &worldGen);
            // Render all of the renderer components here
            ComponentManager::getInstance()->performPhysicsLogic();
            ComponentManager::getInstance()->performRendering();

            if (cameraMode == 0)
            {
                // Move camera by keyboard and cursor
                glfwGetCursorPos(properties.window, &xpos, &ypos);
                camera.rotateView(vec2(xpos / *properties.screenWidth, -ypos / *properties.screenHeight));
                camera.moveCamera(movement, timeDiff * 250.0f);
            }
            else if (cameraMode == 1)
            {
                // Force the camera to follow the vehicle
                // get the velocity of the vehicle
                glm::vec3 velocity = PhysicsEngine::toglmVec3(static_cast<PhysicsComponent*>(playerVehicle->getComponent(PHYSICS))->getRigidBody()->getLinearVelocity());
                glm::vec3 offset = playerVehicle->getForwardVector();

                float dotProd = glm::dot(velocity, playerVehicle->getForwardVector());

                if (dotProd < -10.0f && static_cast<DriveComponent*>(playerVehicle->getComponent(DRIVE))->getBrake()) {
                    movingForward = false;
                } else if (dotProd > -5.0f) {
                    movingForward = true;
                }

                if (!movingForward) offset = -offset;

                camera.rotateCameraTowardPoint(playerVehicle->getPosition() + offset * 10.0f, 10.0f * timeDiff);
                camera.lerpCameraTowardPoint(playerVehicle->getPosition() + offset * -8.0f + glm::vec3(0, 8, 0), 5.0f * timeDiff);
            }

            renderEngine.render(camera);
            audioEngine.simulate();
        }
    }
}
