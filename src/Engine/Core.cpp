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
#include "PhysicsEngine/VehicleConfigParser.h"

GLFWwindow* Core::globalWindow = nullptr;
float Core::simtimeSinceStartup = 0.f;
float Core::realtimeSinceStartup = 0.f;
RenderEngine* Core::renderEngine = nullptr;
std::string Core::dataPath = "data/";
std::vector<Camera*> Core::cameras;

// CHANGE THIS TO FAST STARTUP THE PROGRAM WITH REDUCED GRAPHICS
bool Core::loadOnlyBoxes = true;
// *************************************************************

Core::Core(int *screenWidth,int *screenHeight, GLFWwindow *window, bool gamePaused) {
    //this->properties.openGL_Program = openGL_Program;
    this->properties.window = window;
    this->properties.screenWidth = screenWidth;
    this->properties.screenHeight = screenHeight;
    this->properties.isRunning = true;
    this->properties.isPaused = gamePaused;
    globalWindow = properties.window;

    struct stat info;
    const char* pathname = "data";

    if( stat( pathname, &info ) != 0 ) {
        printf( "No data found in current dir, looking up one folder...\n");
        dataPath = "../data/";
    } else {
        printf("Data found!");
    }
}

Core::~Core() = default;

// there has to be a better way than to make it this way
Movement movement;
int cameraMode = 1;
bool refreshMovement = false;

float physxfpsCounter = 0;
float mainfpsCounter = 0;

float timeSinceLastfpsPrint = 0;

// camera, using keyboard events for WASD
void windowKeyInput(GLFWwindow *window, int key, int scancode, int action, int mods) {
	bool set = action != GLFW_RELEASE && GLFW_REPEAT;
	switch (key) {
	case GLFW_KEY_ESCAPE:
		glfwSetWindowShouldClose(window, GL_TRUE);
		break;
	case GLFW_KEY_UP:
		movement.forward = set ? 1 : 0;
		break;
	case GLFW_KEY_LEFT:
		movement.left = set ? 1 : 0;
		break;
	case GLFW_KEY_DOWN:
		movement.backward = set ? 1 : 0;
		break;
	case GLFW_KEY_RIGHT:
		movement.right = set ? 1 : 0;
		break;
	case GLFW_KEY_SPACE:
		movement.up = set ? 1 : 0;
		break;
	case GLFW_KEY_C:
		movement.down = set ? 1 : 0;
		break;
	}

    if (action == GLFW_RELEASE && key == GLFW_KEY_R) {
        std::string temp = "default";
        VehicleConfigParser::getInstance()->parseConfigFile(temp);
        refreshMovement = true;
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
    renderEngine = new RenderEngine(properties.window, properties.screenWidth, properties.screenHeight);
    AudioEngine audioEngine;
    Logic logic;

	glfwSetKeyCallback(properties.window, windowKeyInput);
	glfwSetInputMode(properties.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // initialize audio engine
    audioEngine.init();
    
    audioEngine.loadSound(Core::dataPath + "sound/bgm1.mp3", false, true, true); // load music
    int musicChannel = audioEngine.playSounds(Core::dataPath + "sound/bgm1.mp3", glm::vec3(0, 0, 0), 1); // play music
    audioEngine.setChannelVolume(musicChannel, -45.f);
    // end audio init

    PhysicsEngine::getInstance()->initPhysics();

    //initialize camera class, in theory if you modify the position/direction of
    // the camera class it will affect the rendered view
	//==========================================================================
	//NOTE: For multiple views it might be a good idea to create and use a camera vector
	//std::vector<Camera*> cameras;
    Camera camera(properties.screenWidth,properties.screenHeight);
	cameras.push_back(&camera);
	//===========================================================================
    //following set of functions opens an object file, I considered sticking an shaderID
    // of some kind into the mesh data (hence the constructor takes a string) but
    // in actually that's never used
//    Model cubeMesh("cubeMesh");
//    cubeMesh.loadMeshData(Core::dataPath + "assets/meshes/cube.obj");

    //Following set of functions adds the shaders to the shader class and then links them
//    ShaderData shaderData;
//    shaderData.attachShader(Core::dataPath + "shaderData/vertex.glsl",GL_VERTEX_SHADER);
//    shaderData.attachShader(Core::dataPath + "shaderData/fragment.glsl",GL_FRAGMENT_SHADER);
//    shaderData.link();


    // Brian's shenanigans go here
    // Don't mind me, just trying to figure out how to shove everything into logic
    logic.bindCamera(&camera);
    // END Brian's shenanigans

    // Create the starting Entities

    //Entity* playerVehicle = EntityManager::getInstance()->createPlayerVehicleEntity(glm::vec3(0, 10, 0));

	//Entity* aiVehicle = EntityManager::getInstance()->createAIVehicleEntity(glm::vec3(10, 10, 0));
    //Entity* aiVehicle2 = EntityManager::getInstance()->createAIVehicleEntity(glm::vec3(20, 10, 0));



	WorldGenerator::getInstance()->generateWorld();
	Entity* playerVehicle = EntityManager::getInstance()->getVehicleEntities().at(0);

    // -----------------End of temp initialize model/instance in rendering code

	double previousTime = glfwGetTime();
    float physicsTime = glfwGetTime();
    const float physicsTimeStep = 1.0f / 60.0f;
	
	// for yaw/pitch controlled by cursor
	double xpos, ypos;
    bool movingForward = true;
	float time;
	while (properties.isRunning && !glfwWindowShouldClose(properties.window)){
        glfwPollEvents();
        realtimeSinceStartup = glfwGetTime();
	    float timeDiff = realtimeSinceStartup - previousTime;
		previousTime = realtimeSinceStartup;
        mainfpsCounter = mainfpsCounter * 0.5f + 1.f / timeDiff * 0.5f;

        // R was pressed, we need to re set the player vehicle
		if (refreshMovement) {
            VehicleConfigParser::getInstance()->applyConfigToVehicle(static_cast<DriveComponent*>(playerVehicle->getComponent(DRIVE))->getVehicle());
		}

        const float playerHealth = static_cast<HealthComponent*>(playerVehicle->getComponent(HEALTH))->getCurrentHealth();

        std::ostringstream oss;
        oss << "Health: " << round(playerHealth);
        std::string playerHealthStr = oss.str();

        renderEngine->ui->modifyText(0, &playerHealthStr, nullptr, nullptr, nullptr, nullptr);

        // Now we need to update the resources collected
        oss.str("");
        oss.clear();
        UpgradeComponent* playerUC = static_cast<UpgradeComponent*>(playerVehicle->getComponent(UPGRADE));
        oss << "Resources: " << roundf(playerUC->getCurrentResources()) << "/" << roundf(playerUC->getMaxResources());
        std::string playerResources = oss.str();
		renderEngine->ui->modifyText(1, &playerResources, nullptr, nullptr, nullptr, nullptr);

		timeSinceLastfpsPrint += timeDiff;
		if (timeSinceLastfpsPrint > 0.2f) {
			timeSinceLastfpsPrint = 0;
			oss.str("");
			oss.clear();
			oss << round(physxfpsCounter);
			std::string physxfpsString = oss.str();
			renderEngine->ui->modifyText(10, &physxfpsString, nullptr, nullptr, nullptr, nullptr);

            oss.str("");
            oss.clear();
            oss << round(mainfpsCounter);
            std::string mainfpsString = oss.str();
            renderEngine->ui->modifyText(11, &mainfpsString, nullptr, nullptr, nullptr, nullptr);
		}

        //We could make a pause game feature by just rendering stuff and disabling all
        // the other stuff... although feel free to change this if you think some other
        // approach is better
        if(properties.isPaused){
            renderEngine->render(cameras);
        }else{
			
			float fixedStepTimediff = 0.0f;
            int current_iter = 0;
			// Simulate physics in a Fixed Timestep style
			while (physicsTime < realtimeSinceStartup && current_iter < MAX_PHYSICS_STEPS_PER_FRAME) {
				physicsTime += physicsTimeStep;
				PhysicsEngine::getInstance()->simulateTimeInSeconds(physicsTimeStep);
				fixedStepTimediff += physicsTimeStep;
                simtimeSinceStartup += physicsTimeStep;
                current_iter++;
			}

            // Prevents the fast forward effect
            if (current_iter == MAX_PHYSICS_STEPS_PER_FRAME) {
                physicsTime = realtimeSinceStartup;
            }

			if (fixedStepTimediff > 0) {
				physxfpsCounter = physxfpsCounter * 0.5f + (1.0f / fixedStepTimediff) * 0.5f;
			}

			logic.playerMovement(playerVehicle);

			for (unsigned int i = 1; i < EntityManager::getInstance()->getVehicleEntities().size(); i++) {
				logic.finiteStateMachine(EntityManager::getInstance()->getVehicleEntities().at(i));
			}

			WorldGenerator::getInstance()->regenerateCrystal();
						
            // Render all of the renderer components here
            ComponentManager::getInstance()->performPhysicsLogic();
            ComponentManager::getInstance()->performProjectileLogic();
            ComponentManager::getInstance()->performRendering();
			
            if (cameraMode == 0)
            {
                // Move camera by keyboard and cursor
                glfwGetCursorPos(properties.window, &xpos, &ypos);
                camera.rotateView(glm::vec2(xpos / *properties.screenWidth, -ypos / *properties.screenHeight));
                camera.moveCamera(movement, fixedStepTimediff * 1000.0f);
            }
            else if (cameraMode == 1)
            {
                // Force the camera to follow the vehicle
                // get the velocity of the vehicle
                glm::vec3 velocity = PhysicsEngine::toglmVec3(static_cast<PhysicsComponent*>(playerVehicle->getComponent(PHYSICS))->getRigidBody()->getLinearVelocity());
                glm::vec3 offset = playerVehicle->getForwardVector();

                float dotProd = glm::dot(velocity, playerVehicle->getForwardVector());

                // Makes the cmaera look behind the car at certain speeds.
                /*if (dotProd < -30.0f && static_cast<DriveComponent*>(playerVehicle->getComponent(DRIVE))->getBrake()) {
                    movingForward = false;
                } else if (dotProd > 5.f) {
                    movingForward = true;
                }*/

                if (!movingForward) offset = -offset;

                const float chassisLevel = static_cast<UpgradeComponent*>(playerVehicle->getComponent(UPGRADE))->getChassisLevel();

                camera.rotateCameraTowardPoint(playerVehicle->getPosition() + offset * 10.0f, 10.0f * fixedStepTimediff);
                camera.lerpCameraTowardPoint(playerVehicle->getPosition() + offset * -12.0f * chassisLevel + glm::vec3(0, 8 + 4.f * (chassisLevel - 0.5f), 0), 10.0f * fixedStepTimediff);
            }

            audioEngine.update();
            renderEngine->render(cameras);
        }
    }
}
