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
#include "Importer/Managers/TextureDataManager.h"

GLFWwindow* Core::globalWindow = nullptr;
float Core::simtimeSinceStartup = 0.f;
float Core::realtimeSinceStartup = 0.f;
RenderEngine* Core::renderEngine = nullptr;
std::string Core::dataPath = "data/";
std::vector<Camera*> Core::cameras;

// CHANGE THIS TO FAST STARTUP THE PROGRAM WITH REDUCED GRAPHICS
bool Core::loadOnlyBoxes = false;
// *************************************************************

Core::Core(int *screenWidth,int *screenHeight, GLFWwindow *window, bool gamePaused) {
    //this->properties.openGL_Program = openGL_Program;
    this->properties.window = window;
    this->properties.screenWidth = screenWidth;
    this->properties.screenHeight = screenHeight;
    this->properties.isRunning = true;
    this->properties.isPaused = gamePaused;
    this->properties.inMainMenu = true;
    this->properties.isGameInitialized = true;
    this->properties.isIngameMenuInitialized = false;
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

float physxIterCounter = 0;
float mainfpsCounter = 0;

float timeSinceLastfpsPrint = 0;

bool keyPressedUp = false;
bool keyPressedDown = false;
bool enterPressed = false;
bool pauseButtonPressed = false;

// camera, using keyboard events for WASD
void windowKeyInput(GLFWwindow *window, int key, int scancode, int action, int mods) {
	bool set = action != GLFW_RELEASE && GLFW_REPEAT;
	switch (key) {
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

	// Controls whether the camera is free or locked to the player vehicle
	if (key == GLFW_KEY_1) {
		cameraMode = 1;
	} else if (key == GLFW_KEY_2) {
		cameraMode = 0;
	}

    keyPressedUp = (key == GLFW_KEY_UP || key == GLFW_KEY_W) && action == GLFW_RELEASE;
    keyPressedDown = (key == GLFW_KEY_DOWN || key == GLFW_KEY_S) && action == GLFW_RELEASE;
    enterPressed = key == GLFW_KEY_ENTER && action == GLFW_RELEASE;
    pauseButtonPressed = key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE;
}

float timeDiff = 0;
AudioEngine* audioEngine = nullptr;
Logic* logic = nullptr;

//Main game loop
void Core::coreLoop() {
    renderEngine = new RenderEngine(properties.window, properties.screenWidth, properties.screenHeight);
    audioEngine = new AudioEngine;
    logic = new Logic;

	glfwSetKeyCallback(properties.window, windowKeyInput);
	glfwSetInputMode(properties.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // initialize audio engine
    audioEngine->init();
    
    audioEngine->loadSound(Core::dataPath + "sound/bgm1.mp3", false, true, true); // load music
    int musicChannel = audioEngine->playSounds(Core::dataPath + "sound/bgm1.mp3", glm::vec3(0, 0, 0), 1); // play music
    audioEngine->setChannelVolume(musicChannel, -45.f);
    // end audio init

    PhysicsEngine::getInstance()->initPhysics();

    Camera camera(properties.screenWidth,properties.screenHeight);
	cameras.push_back(&camera);

    logic->bindCamera(&camera);

    // -----------------End of temp initialize model/instance in rendering code

	double previousTime = glfwGetTime();

	while (properties.isRunning && !glfwWindowShouldClose(properties.window)){
        glfwPollEvents();
        realtimeSinceStartup = glfwGetTime();
	    timeDiff = realtimeSinceStartup - previousTime;
		previousTime = realtimeSinceStartup;
        mainfpsCounter = mainfpsCounter * 0.5f + 1.f / timeDiff * 0.5f;

        
        if (properties.inMainMenu) {
            runMenu();
        } else {
            runGame();
        }
        audioEngine->update();
        renderEngine->render(cameras);
    }
}

float physicsTime = glfwGetTime();
const float physicsTimeStep = 1.0f / 60.0f;

// for yaw/pitch controlled by cursor
double xpos, ypos;
bool movingForward = true;
Entity* playerVehicle = nullptr;

int physxIterCounterId = -1;
int mainfpsCounterId = -1;
int playerHealthId = -1;
int playerResourceId = -1;

bool controllerButtonPressed = false;
#define     GAMEPAD_START	7

void Core::runGame() {
    if (properties.isGameInitialized == false) {
        // We need to clear the menu and all of its buttons and spawn the UI/Game elements for our scene
        renderEngine->ui->clearAllUI();
        EntityManager::getInstance()->destroyAllEntities();

        physxIterCounterId = renderEngine->ui->addText("78", 5, 5, 0.5, glm::vec3(0, 1, 0));
        mainfpsCounterId = renderEngine->ui->addText("100", 50, 5, 0.5, glm::vec3(1, 1, 0));

        // Health and resource text
        playerHealthId = renderEngine->ui->addText("health", 40, 40, 1, glm::vec3(0.5, 1, 0));
        playerResourceId = renderEngine->ui->addText("resources", 1550, 30, 0.5, glm::vec3(1, 1, 0));

        // Create the upgrade text
        logic->createPlayerUpgradeUI();

        WorldGenerator::getInstance()->generateWorld();
        playerVehicle = EntityManager::getInstance()->getVehicleEntities().at(0);
        properties.isGameInitialized = true;

        // Last thing to do is correctly set the time
        realtimeSinceStartup = glfwGetTime();
        physicsTime = realtimeSinceStartup;
    }

    // ***************************************** START OF GAME SPECIFIC CODE *******************************************************
    const float playerHealth = static_cast<HealthComponent*>(playerVehicle->getComponent(HEALTH))->getCurrentHealth();

    // Check to see if the game should pause:
    int present = glfwJoystickPresent(GLFW_JOYSTICK_1);
    if (present) {
        int buttonCount;
        const unsigned char *buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);
        if (buttons[GAMEPAD_START]) {
            if (!controllerButtonPressed) {
                pauseButtonPressed = true;
            }
            controllerButtonPressed = true;
        } else {
            controllerButtonPressed = false;
        }
    }
    // Only set this value to false if the game has yet to be paused. Also used for unpausing.
    if (pauseButtonPressed && properties.isPaused == false) {
        pauseButtonPressed = false;
        properties.isPaused = true;
    }

    std::ostringstream oss;
    oss << "Health: " << round(playerHealth);
    std::string playerHealthStr = oss.str();

    renderEngine->ui->modifyText(playerHealthId, &playerHealthStr, nullptr, nullptr, nullptr, nullptr);

    // Now we need to update the resources collected
    oss.str("");
    oss.clear();
    UpgradeComponent* playerUC = static_cast<UpgradeComponent*>(playerVehicle->getComponent(UPGRADE));
    oss << "Resources: " << roundf(playerUC->getCurrentResources()) << "/" << roundf(playerUC->getMaxResources());
    std::string playerResources = oss.str();
    renderEngine->ui->modifyText(playerResourceId, &playerResources, nullptr, nullptr, nullptr, nullptr);

    timeSinceLastfpsPrint += timeDiff;
    if (timeSinceLastfpsPrint > 0.2f) {
        timeSinceLastfpsPrint = 0;
        oss.str("");
        oss.clear();
        oss << (round(physxIterCounter * 10.f) / 10.f);
        std::string physxfpsString = oss.str();
        renderEngine->ui->modifyText(physxIterCounterId, &physxfpsString, nullptr, nullptr, nullptr, nullptr);

        oss.str("");
        oss.clear();
        oss << round(mainfpsCounter);
        std::string mainfpsString = oss.str();
        renderEngine->ui->modifyText(mainfpsCounterId, &mainfpsString, nullptr, nullptr, nullptr, nullptr);
    }

    if(properties.isPaused){
        runPauseMenu();
    }else{

        float fixedStepTimediff = 0.0f;
        int current_iter = 0;
        float physxIters = 0;
        // Simulate physics in a Fixed Timestep style
        while (physicsTime < realtimeSinceStartup && current_iter < MAX_PHYSICS_STEPS_PER_FRAME) {
            physicsTime += physicsTimeStep;
            PhysicsEngine::getInstance()->simulateTimeInSeconds(physicsTimeStep);
            fixedStepTimediff += physicsTimeStep;
            simtimeSinceStartup += physicsTimeStep;
            current_iter++;
            physxIters++;
        }

        // Calculate the physx updates per frame
        physxIterCounter = (physxIterCounter * 0.5f) + (physxIters * 0.5f);

        // Prevents the fast forward effect
        if (current_iter == MAX_PHYSICS_STEPS_PER_FRAME) {
            physicsTime = realtimeSinceStartup;
        }

        logic->playerMovement(playerVehicle);

        for (unsigned int i = 1; i < EntityManager::getInstance()->getVehicleEntities().size(); i++) {
            logic->finiteStateMachine(EntityManager::getInstance()->getVehicleEntities().at(i));
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
            cameras[0]->rotateView(glm::vec2(xpos / *properties.screenWidth, -ypos / *properties.screenHeight));
            cameras[0]->moveCamera(movement, fixedStepTimediff * 1000.0f);
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

            cameras[0]->rotateCameraTowardPoint(playerVehicle->getPosition() + offset * 10.0f, 10.0f * fixedStepTimediff);
            cameras[0]->lerpCameraTowardPoint(playerVehicle->getPosition() + offset * -12.0f * chassisLevel + glm::vec3(0, 8 + 4.f * (chassisLevel - 0.5f), 0), 10.0f * fixedStepTimediff);
        }
    }
}

// **************************** Variables for menu ********************************
std::vector<int> currentImageUiIds;
std::vector<int> currentTextUiIds;

std::vector<int> constantTextUI;
std::vector<int> constantImageUI;
Entity* mainMenuEnt = nullptr;

enum MainMenuState {NONE, MAINMENU, OPTIONS, GAMEMODES};
MainMenuState currentMainMenuState = NONE;
MainMenuState nextMainMenuState = MAINMENU;
int currentChoiceIndex = 0;
int maxChoiceIndex = 1;

#define     GAMEPAD_X		2
#define 	GAMEPAD_DPAD_UP   10
#define 	GAMEPAD_DPAD_DOWN   12


bool previouslyPressed = false;

void Core::runMenu() {

    if (properties.isGameInitialized) {
        // If the game has been initialized, then we need to completely clear it before we do anything else.
        renderEngine->ui->clearAllUI();
        EntityManager::getInstance()->destroyAllEntities();

        currentMainMenuState = NONE;
        nextMainMenuState = MAINMENU;

        renderEngine->ui->addText("KARSIO", 90, 140, 2, glm::vec3(0.7, 0.7, 0), 1);
        currentChoiceIndex = 0;
        

        // Now render the spinning vehicle
        mainMenuEnt = EntityManager::getInstance()->createBasicVehicleEntity(glm::vec3(5, 0, 0));
        properties.isGameInitialized = false;
    }

    // If we clicked something and change the menu state, then load the new menu
    if (currentMainMenuState != nextMainMenuState) {
        currentMainMenuState = nextMainMenuState;

        for (int i = 0; i < currentImageUiIds.size(); ++i) {
            renderEngine->ui->removeImage(currentImageUiIds[i]);
        }
        currentImageUiIds.clear();
        for (int i = 0; i < currentTextUiIds.size(); ++i) {
            renderEngine->ui->removeText(currentTextUiIds[i]);
        }
        currentTextUiIds.clear();
        currentChoiceIndex = 0;
        switch (currentMainMenuState) {
        case OPTIONS:
            currentImageUiIds.push_back(renderEngine->ui->addImage(*TextureDataManager::getImageData("button1Small.jpg"), 100, 600));
            
            currentTextUiIds.push_back(renderEngine->ui->addText("Back", 140, 630, 1, glm::vec3(1, 1, 0), 1));
            currentTextUiIds.push_back(renderEngine->ui->addText("Options are TBD.", 100, 400, 1, glm::vec3(1, 1, 0), 1));
            maxChoiceIndex = 1;
            break;
        case GAMEMODES:
            currentImageUiIds.push_back(renderEngine->ui->addImage(*TextureDataManager::getImageData("button1Small.jpg"), 100, 300));
            currentImageUiIds.push_back(renderEngine->ui->addImage(*TextureDataManager::getImageData("button1Small.jpg"), 100, 600));

            currentTextUiIds.push_back(renderEngine->ui->addText("Survival", 140, 330, 1, glm::vec3(0, 1, 0), 1));
            currentTextUiIds.push_back(renderEngine->ui->addText("Back", 140, 630, 1, glm::vec3(1, 1, 0), 1));
            maxChoiceIndex = 2;
            break;
        default: // Default is the MAINMENU
            currentImageUiIds.push_back(renderEngine->ui->addImage(*TextureDataManager::getImageData("button1Small.jpg"), 100, 300));
            currentImageUiIds.push_back(renderEngine->ui->addImage(*TextureDataManager::getImageData("button1Small.jpg"), 100, 450));
            currentImageUiIds.push_back(renderEngine->ui->addImage(*TextureDataManager::getImageData("button1Small.jpg"), 100, 600));

            currentTextUiIds.push_back(renderEngine->ui->addText("Play Game", 140, 330, 1, glm::vec3(0, 1, 0), 1));
            currentTextUiIds.push_back(renderEngine->ui->addText("Options", 140, 480, 1, glm::vec3(1, 1, 0), 1));
            currentTextUiIds.push_back(renderEngine->ui->addText("Exit", 140, 630, 1, glm::vec3(1, 1, 0), 1));
            maxChoiceIndex = 3;
            break;
        }
    }

    // Process user input to change the selection: 
    // CONTROLLER IMPLEMENTATION
    int present = glfwJoystickPresent(GLFW_JOYSTICK_1);
    if (present) {
        int buttonCount;
        const unsigned char *buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);
        if (buttons[GAMEPAD_DPAD_DOWN]) {
            if (!previouslyPressed) {
                currentChoiceIndex = (currentChoiceIndex + 1) % maxChoiceIndex;
            }
            previouslyPressed = true;
        } else if (buttons[GAMEPAD_DPAD_UP]) {
            if (!previouslyPressed) {
                currentChoiceIndex = currentChoiceIndex == 0 ? maxChoiceIndex - 1: currentChoiceIndex - 1;
            }
            previouslyPressed = true;
        } else if (buttons[GAMEPAD_X]) {
            if (!previouslyPressed) {
                enterPressed = true;
            }
            previouslyPressed = true;
        } else {
            previouslyPressed = false;
        }
    } else { // KEYBOARD IMPLEMENTATION
        if (keyPressedUp) {
            currentChoiceIndex = currentChoiceIndex == 0 ? maxChoiceIndex - 1: currentChoiceIndex - 1;
            keyPressedUp = false;
        } else if (keyPressedDown) {
            currentChoiceIndex = (currentChoiceIndex + 1) % maxChoiceIndex;
            keyPressedDown = false;
        }
    }

    if (enterPressed) {
        enterPressed = false;
        switch (currentMainMenuState) {
            case MAINMENU:
                if (currentChoiceIndex == 0) {
                    nextMainMenuState = GAMEMODES;
                } else if (currentChoiceIndex == 1) {
                    nextMainMenuState = OPTIONS;
                } else if (currentChoiceIndex == 2) {
                    glfwSetWindowShouldClose(globalWindow, GL_TRUE);
                    properties.isRunning = false;
                }
            break;
            case OPTIONS:
                if (currentChoiceIndex == 0) {
                    nextMainMenuState = MAINMENU;
                }
            break;
            case GAMEMODES:
                if (currentChoiceIndex == 0) {
                    // Time to launch the actual game
                    properties.inMainMenu = false;
                } else if (currentChoiceIndex == 1) {
                    nextMainMenuState = MAINMENU;
                }
            break;
            default:
                nextMainMenuState = MAINMENU;
            break;
        }
        currentChoiceIndex = 0;
    }

    // Make the current selection green
    for (int i = 0; i < currentTextUiIds.size(); ++i) {
        if (i == currentChoiceIndex) {
            renderEngine->ui->modifyText(currentTextUiIds[i],nullptr, nullptr, nullptr, nullptr, &glm::vec3(0,1,0), nullptr);
        } else {
            renderEngine->ui->modifyText(currentTextUiIds[i],nullptr, nullptr, nullptr, nullptr, &glm::vec3(1,1,0), nullptr);
        }
    }

    // Make the camera look at the vehicle
    cameras[0]->rotateCameraTowardPoint(mainMenuEnt->getPosition(), 5.f * timeDiff);
    cameras[0]->lerpCameraTowardPoint(glm::vec3(-20,10,0), 5.0f * timeDiff);

    // We need to render the UI if this is the case and keep track of where we are/what is selected.
    UpgradeComponent* uc = static_cast<UpgradeComponent*>(mainMenuEnt->getComponent(UPGRADE));
    uc->addResources(timeDiff * 50.f);

    if (uc->isUpgradeAvailable()) {
        UpgradeType type = UpgradeType(logic->randomNum(0, 3));
        while (!uc->canUpgradeType(type)) {
            type = UpgradeType(logic->randomNum(0, 3));
        }
        uc->upgradeVehicle(type);
    }

    static_cast<PhysicsComponent*>(mainMenuEnt->getComponent(PHYSICS))->getRigidBody()->setGlobalPose(physx::PxTransform(physx::PxVec3(5, 0, 0),physx::PxQuat(realtimeSinceStartup, physx::PxVec3(0,1,0))));

    // Update the physics and rendering aspects of the simulation
    PhysicsEngine::getInstance()->simulateTimeInSeconds(0.0);
    ComponentManager::getInstance()->performPhysicsLogic();
    ComponentManager::getInstance()->performRendering();
}

enum PauseMenuState {PAUSENONE, PAUSEMAIN, PAUSEOPTIONS};
PauseMenuState currentPauseMenuState = PAUSENONE;
PauseMenuState nextPauseMenuState = PAUSEMAIN;

void Core::runPauseMenu() {
    

    if (!properties.isIngameMenuInitialized) {

        currentPauseMenuState = PAUSENONE;
        nextPauseMenuState = PAUSEMAIN;
        currentChoiceIndex = 0;

        constantTextUI.push_back(renderEngine->ui->addText("Game Paused", 75, 225, 1.25, glm::vec3(1, 1, 0), 1));
        properties.isIngameMenuInitialized = true;
    }

    // If we clicked something and change the menu state, then load the new menu
    if (currentPauseMenuState != nextPauseMenuState) {
        currentPauseMenuState = nextPauseMenuState;

        for (int i = 0; i < currentImageUiIds.size(); ++i) {
            renderEngine->ui->removeImage(currentImageUiIds[i]);
        }
        currentImageUiIds.clear();
        for (int i = 0; i < currentTextUiIds.size(); ++i) {
            renderEngine->ui->removeText(currentTextUiIds[i]);
        }
        currentTextUiIds.clear();
        currentChoiceIndex = 0;
        switch (currentPauseMenuState) {
        case PAUSEOPTIONS:
            currentImageUiIds.push_back(renderEngine->ui->addImage(*TextureDataManager::getImageData("button1Small.jpg"), 100, 600));
            currentImageUiIds.push_back(renderEngine->ui->addImage(*TextureDataManager::getImageData("Panel1Small.jpg"), 50, 200));

            currentTextUiIds.push_back(renderEngine->ui->addText("Back", 140, 630, 1, glm::vec3(1, 1, 0), 1));
            currentTextUiIds.push_back(renderEngine->ui->addText("Options are TBD.", 75, 400, 1, glm::vec3(1, 1, 0), 1));
            maxChoiceIndex = 1;
            break;
        default: // Default is the PAUSEMENU
            currentImageUiIds.push_back(renderEngine->ui->addImage(*TextureDataManager::getImageData("button1Small.jpg"), 100, 300));
            currentImageUiIds.push_back(renderEngine->ui->addImage(*TextureDataManager::getImageData("button1Small.jpg"), 100, 450));
            currentImageUiIds.push_back(renderEngine->ui->addImage(*TextureDataManager::getImageData("button1Small.jpg"), 100, 600));
            currentImageUiIds.push_back(renderEngine->ui->addImage(*TextureDataManager::getImageData("Panel1Small.jpg"), 50, 200));

            currentTextUiIds.push_back(renderEngine->ui->addText("Resume", 140, 330, 1, glm::vec3(0, 1, 0), 1));
            currentTextUiIds.push_back(renderEngine->ui->addText("Options", 140, 480, 1, glm::vec3(1, 1, 0), 1));
            currentTextUiIds.push_back(renderEngine->ui->addText("Main Menu", 140, 630, 1, glm::vec3(1, 1, 0), 1));
            maxChoiceIndex = 3;
            break;
        }
    }

    // Process user input to change the selection: 
    // CONTROLLER IMPLEMENTATION
    int present = glfwJoystickPresent(GLFW_JOYSTICK_1);
    if (present) {
        int buttonCount;
        const unsigned char *buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);
        if (buttons[GAMEPAD_DPAD_DOWN]) {
            if (!previouslyPressed) {
                currentChoiceIndex = (currentChoiceIndex + 1) % maxChoiceIndex;
            }
            previouslyPressed = true;
        } else if (buttons[GAMEPAD_DPAD_UP]) {
            if (!previouslyPressed) {
                currentChoiceIndex = currentChoiceIndex == 0 ? maxChoiceIndex - 1: currentChoiceIndex - 1;
            }
            previouslyPressed = true;
        } else if (buttons[GAMEPAD_X]) {
            if (!previouslyPressed) {
                enterPressed = true;
            }
            previouslyPressed = true;
        } else {
            previouslyPressed = false;
        }
    } else { // KEYBOARD IMPLEMENTATION
        if (keyPressedUp) {
            currentChoiceIndex = currentChoiceIndex == 0 ? maxChoiceIndex - 1: currentChoiceIndex - 1;
            keyPressedUp = false;
        } else if (keyPressedDown) {
            currentChoiceIndex = (currentChoiceIndex + 1) % maxChoiceIndex;
            keyPressedDown = false;
        }
    }

    // If the pause button is hit again then unpause the game
    if (pauseButtonPressed) {
        printf("Pause button was pressed!\n");
        pauseButtonPressed = false;
        enterPressed = true;
        currentPauseMenuState = PAUSEMAIN;
        currentChoiceIndex = 0;
    }

    if (enterPressed) {
        enterPressed = false;
        switch (currentPauseMenuState) {
        case PAUSEMAIN:
            if (currentChoiceIndex == 0) {
                // UNPAUSE
                properties.isPaused = false;
                properties.isIngameMenuInitialized = false;

                // Now I need to erase the popup menu
                for (int i = 0; i < currentImageUiIds.size(); ++i) {
                    renderEngine->ui->removeImage(currentImageUiIds[i]);
                }
                currentImageUiIds.clear();
                for (int i = 0; i < currentTextUiIds.size(); ++i) {
                    renderEngine->ui->removeText(currentTextUiIds[i]);
                }
                currentTextUiIds.clear();
                for (int i = 0; i < constantImageUI.size(); ++i) {
                    renderEngine->ui->removeImage(constantImageUI[i]);
                }
                constantImageUI.clear();
                for (int i = 0; i < constantTextUI.size(); ++i) {
                    renderEngine->ui->removeText(constantTextUI[i]);
                }
                constantTextUI.clear();

                // Set the time so funky stuff doesn't happen
                realtimeSinceStartup = glfwGetTime();
                physicsTime = realtimeSinceStartup;

            } else if (currentChoiceIndex == 1) {
                nextPauseMenuState = PAUSEOPTIONS;
            } else if (currentChoiceIndex == 2) {
                properties.isPaused = false;
                properties.isIngameMenuInitialized = false;
                properties.inMainMenu = true;
            }
            break;
        case PAUSEOPTIONS:
            if (currentChoiceIndex == 0) {
                nextPauseMenuState = PAUSEMAIN;
            }
            break;
        default:
            nextPauseMenuState = PAUSEMAIN;
            break;
        }
        currentChoiceIndex = 0;
    }

    // Make the current selection green
    for (int i = 0; i < currentTextUiIds.size(); ++i) {
        if (i == currentChoiceIndex) {
            renderEngine->ui->modifyText(currentTextUiIds[i],nullptr, nullptr, nullptr, nullptr, &glm::vec3(0,1,0), nullptr);
        } else {
            renderEngine->ui->modifyText(currentTextUiIds[i],nullptr, nullptr, nullptr, nullptr, &glm::vec3(1,1,0), nullptr);
        }
    }
}
