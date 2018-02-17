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

Core::Core(int *screenWidth,int *screenHeight, GLFWwindow *window, bool gamePaused) {
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

void joystick(const unsigned char *buttons, const float *axes) {
	
	if (buttons[0] == GLFW_PRESS) {			// A
		
	}
	if (buttons[1] == GLFW_PRESS) {			// B

	}
	if (buttons[2] == GLFW_PRESS) {			// X
		
	}
	if (buttons[3] == GLFW_PRESS) {			// Y

	}
	if (buttons[4] == GLFW_PRESS) {			// LB
		
	}
	if (buttons[5] == GLFW_PRESS) {			// RB
		
	}
	if (buttons[6] == GLFW_PRESS) {			// BACK
		
	}
	if (buttons[7] == GLFW_PRESS) {			// START

	}

	//std::cout << "left x joystick: " << axes[0] << std::endl;
	//std::cout << "left y joystick: " << axes[1] << std::endl;
	//std::cout << "right x joystick: " << axes[2] << std::endl;
	//std::cout << "right y joystick: " << axes[3] << std::endl;
	//std::cout << "l trigger " << axes[4] << std::endl;
	//std::cout << "r trigger " << axes[5] << std::endl;
	
	// right x/y axis (axes[2], axes[3] respectively) to be used for gun aiming

	tempPlayerInput.brake = (axes[4] > 0) ? true : false;	// LEFT TRIGGER
	tempPlayerInput.accel = (axes[5] > 0) ?  true : false;	// RIGHT TRIGGER
	tempPlayerInput.steerLeft = (axes[0] > 0) ? true : false; // LEFT X JOYSTICK
	tempPlayerInput.steerRight = (axes[0] < 0) ? true : false; // LEFT X JOYSTICK
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

	// Create some obstacles										// location				size
	Entity* obstacle1 = EntityManager::getInstance()->createBox(glm::vec3(5.f, 1.f, 25.f), glm::vec3(1.f));
	Entity* obstacle2 = EntityManager::getInstance()->createBox(glm::vec3(6.f, 1.f, 25.f), glm::vec3(1.f));
	Entity* obstacle3 = EntityManager::getInstance()->createBox(glm::vec3(7.f, 1.f, 25.f), glm::vec3(1.f));
	Entity* obstacle4 = EntityManager::getInstance()->createBox(glm::vec3(8.f, 1.f, 25.f), glm::vec3(1.f));
	Entity* obstacle5 = EntityManager::getInstance()->createBox(glm::vec3(9.f, 1.f, 25.f), glm::vec3(1.f));
	Entity* obstacle6 = EntityManager::getInstance()->createBox(glm::vec3(10.f, 1.f, 25.f), glm::vec3(1.f));
	
    // Create the starting Entities
    Entity* playerVehicle = EntityManager::getInstance()->createBasicVehicleEntity(glm::vec3(0, 1, 0));
	Entity* aiVehicle = EntityManager::getInstance()->createAIVehicleEntity(glm::vec3(10, 1, 0));
	Entity* groundPlane = EntityManager::getInstance()->createGroundPlane();
	
    // Creating crytsal entities. A value for the size of the crystal can be speicifed if wanted
    Entity* crystalEntity1 = EntityManager::getInstance()->createCrystal(glm::vec3(5, 1.0f, 5), 0.5f);
    Entity* crystalEntity2 = EntityManager::getInstance()->createCrystal(glm::vec3(5, 1.0f, 15), 1.0f);
    Entity* crystalEntity3 = EntityManager::getInstance()->createCrystal(glm::vec3(5, 1.0f, 25), 1.5f);
    Entity* crystalEntity4 = EntityManager::getInstance()->createCrystal(glm::vec3(5, 1.0f, 35), 2.0f);
    Entity* crystalEntity5 = EntityManager::getInstance()->createCrystal(glm::vec3(5, 1.0f, 45), 2.5f);

    // These crystals mark the ai path
    Entity* crystalEntity6 = EntityManager::getInstance()->createCrystal(glm::vec3(40, 1.0f, 40), 0.5f);
    Entity* crystalEntity7 = EntityManager::getInstance()->createCrystal(glm::vec3(-50, 1.0f, 50), 0.65f);
    Entity* crystalEntity8 = EntityManager::getInstance()->createCrystal(glm::vec3(-50, 1.0f, -50), 0.85f);
    Entity* crystalEntity9 = EntityManager::getInstance()->createCrystal(glm::vec3(50, 1.0f, -50), 1.0f);

    // Create some temporary walls around the outside to prevent people from escaping.
    Entity* wall1 = EntityManager::getInstance()->createBox(glm::vec3(-100,2,0), glm::vec3(2, 4, 100));
    Entity* wall2 = EntityManager::getInstance()->createBox(glm::vec3(100, 2, 0), glm::vec3(2, 4, 100));
    Entity* wall3 = EntityManager::getInstance()->createBox(glm::vec3(0, 2, 100), glm::vec3(100, 4, 2));
    Entity* wall4 = EntityManager::getInstance()->createBox(glm::vec3(0, 2, -100), glm::vec3(100, 4, 2));

	std::vector<Entity*> obstacles;
	obstacles.push_back(obstacle1);
	obstacles.push_back(obstacle2);
	obstacles.push_back(obstacle3);
	obstacles.push_back(obstacle4);
	obstacles.push_back(obstacle5);
	obstacles.push_back(obstacle6);
	obstacles.push_back(crystalEntity1);
	obstacles.push_back(crystalEntity2);
	obstacles.push_back(crystalEntity3);
	obstacles.push_back(crystalEntity4);
	obstacles.push_back(crystalEntity5);
	//obstacles.push_back(crystalEntity6);
	//obstacles.push_back(crystalEntity7);
	obstacles.push_back(crystalEntity8);
	obstacles.push_back(crystalEntity9);
	
	for (int i = 1; i < 100; i++) {
		Entity* obstacle = EntityManager::getInstance()->createBox(glm::vec3(-70+i, 1.f, 25.f), glm::vec3(.5f));
		obstacles.push_back(obstacle);
	}

	AStar::Generator gen;
	gen.setWorldSize({ 15, 15 });

	float x, y;
	for (int i = 0; i < obstacles.size() - 1; i++) {
		x = obstacles[i]->getCoarsePosition().x;
		y = obstacles[i]->getCoarsePosition().z;
		gen.addCollision(vec2(x, y));
	}
	logic.findPath(&gen, aiVehicle, crystalEntity7);
	
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
	
		// joystick controller stuff
		int present = glfwJoystickPresent(GLFW_JOYSTICK_1);
		if (present == 1) {
			int axesCount;
			const float *axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axesCount);
			int buttonCount;
			const unsigned char *buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);
			joystick(buttons, axes);
		}

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
			//std::cout << "current position " << playerVehicle->getCoarsePosition().x << " " << playerVehicle->getCoarsePosition().z << std::endl;
			logic.playerMovement(&tempPlayerInput, playerVehicle);
            logic.aiMovement(aiVehicle);

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

                if (dotProd < -10.0f && tempPlayerInput.brake) {
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
