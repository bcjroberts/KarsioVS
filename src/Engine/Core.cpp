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
float Core::timeSinceStartup = 0.f;
RenderEngine* Core::renderEngine = nullptr;
std::string Core::dataPath = "data/";

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
        printf( "No data found, looking up one folder...\n");
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
        VehicleConfigParser::getInstance()->parseConfigFile();
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
    audioEngine.setChannelVolume(musicChannel, -10.f);
    // end audio init

    PhysicsEngine::getInstance()->initPhysics();

    //initialize camera class, in theory if you modify the position/direction of
    // the camera class it will affect the rendered view
    Camera camera(properties.screenWidth,properties.screenHeight);

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
    Entity* playerVehicle = EntityManager::getInstance()->createPlayerVehicleEntity(glm::vec3(0, 10, 0));
	Entity* aiVehicle = EntityManager::getInstance()->createAIVehicleEntity(glm::vec3(10, 10, 0));
    Entity* aiVehicle2 = EntityManager::getInstance()->createAIVehicleEntity(glm::vec3(20, 10, 0));

	WorldGenerator worldGen;
	worldGen.generateWorld();
		
    ComponentManager::getInstance()->initializeRendering(renderEngine);
    // -----------------End of temp initialize model/instance in rendering code

	double previousTime = 0;
    float physicsTime = 0;
    const float physicsTimeStep = 1.0f / 60.0f;
	
	// for yaw/pitch controlled by cursor
	double xpos, ypos;
    bool movingForward = true;

	while (properties.isRunning && !glfwWindowShouldClose(properties.window)){
        glfwPollEvents();
        timeSinceStartup = glfwGetTime();

		const auto currentTime = timeSinceStartup;
	    float timeDiff = currentTime - previousTime;
		previousTime = currentTime;
	
        // R was pressed, we need to re set the player vehicle
		if (refreshMovement) {
            VehicleConfigParser::getInstance()->applyConfigToVehicle(static_cast<DriveComponent*>(playerVehicle->getComponent(DRIVE))->getVehicle());
		}

        const float playerHealth = static_cast<HealthComponent*>(playerVehicle->getComponent(HEALTH))->getCurrentHealth();
        //const float AIHealth = static_cast<HealthComponent*>(aiVehicle->getComponent(HEALTH))->getCurrentHealth();
        //printf("PlayerHealth: %f AIHealth: %f\n",playerHealth, AIHealth);

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

        //We could make a pause game feature by just rendering stuff and disabling all
        // the other stuff... although feel free to change this if you think some other
        // approach is better
        if(properties.isPaused){
            renderEngine->render(camera);
        }else{
            
			timeDiff = 0.0f;
			// Simulate physics in a Fixed Timestep style
			while (physicsTime < currentTime) {
				physicsTime += physicsTimeStep;
				PhysicsEngine::getInstance()->simulateTimeInSeconds(physicsTimeStep);
				timeDiff += 1.0f / 60.0f;
			}
						
			logic.playerMovement(playerVehicle);
			logic.finiteStateMachine(aiVehicle, worldGen.getGrid(), &worldGen);
            logic.finiteStateMachine(aiVehicle2, worldGen.getGrid(), &worldGen);

            // Render all of the renderer components here
            ComponentManager::getInstance()->performPhysicsLogic();
            ComponentManager::getInstance()->performProjectileLogic();
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

                if (dotProd < -30.0f && static_cast<DriveComponent*>(playerVehicle->getComponent(DRIVE))->getBrake()) {
                    movingForward = false;
                } else if (dotProd > 5.f) {
                    movingForward = true;
                }

                if (!movingForward) offset = -offset;

                const float chassisLevel = static_cast<UpgradeComponent*>(playerVehicle->getComponent(UPGRADE))->getChassisLevel();

                camera.rotateCameraTowardPoint(playerVehicle->getPosition() + offset * 10.0f, 10.0f * timeDiff);
                camera.lerpCameraTowardPoint(playerVehicle->getPosition() + offset * -8.0f * chassisLevel + glm::vec3(0, 8 + 4.f * (chassisLevel - 1.f), 0), 5.0f * timeDiff);
            }

            audioEngine.update();
            renderEngine->render(camera);
        }
    }
}
