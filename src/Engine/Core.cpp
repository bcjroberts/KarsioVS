//
// The main game loop that (hopefully) refreshes at least 60 times a second
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
#include "AudioObservable.h"
#include "../Game/Components/DriveComponent.h"
#include "../Game/Logic/WorldGenerator.h"
#include "PhysicsEngine/VehicleConfigParser.h"
#include "Importer/Managers/TextureDataManager.h"
#include "AudioPaths.h"
#include <glm/gtx/vector_angle.inl>
GLFWwindow* Core::globalWindow = nullptr;
float Core::simtimeSinceStartup = 0.f;
float Core::realtimeSinceStartup = 0.f;
int Core::upgradeLizardId = -1;
RenderEngine* Core::renderEngine = nullptr;
std::string Core::dataPath = "data/";
std::vector<Camera*> Core::cameras;

//glm::vec3 Core::menuBaseTextColor(0,0,0);
//glm::vec3 Core::menuSelectedTextColor(0,1,0);
// string names rather than paths

glm::vec3 Core::menuBaseTextColor(0.13, 0.09, 0.28);
glm::vec3 Core::menuSelectedTextColor(0.82, 0.8, 1);

AudioObservable* audioEvents;

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
	this->properties.isInUpgradeMenu = false;
    globalWindow = properties.window;

    struct stat info;
    const char* pathname = "data";

    if( stat( pathname, &info ) != 0 ) {
        printf( "No data found in current dir, looking up one folder...\n");
        dataPath = "../data/";
    } else {
        printf("Data found!\n");
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
bool forceReplay = false;
bool upgradeButtonPressed = false;
bool changeTargetPressed = false;

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
    forceReplay = key == GLFW_KEY_Y && action == GLFW_RELEASE;
	upgradeButtonPressed = key == GLFW_KEY_X && action == GLFW_RELEASE;
	changeTargetPressed = key == GLFW_KEY_E && action == GLFW_RELEASE;
}

float timeDiff = 0;
AudioEngine* audioEngine = nullptr;
AudioObserver* audioObserver = nullptr;
Logic* logic = nullptr;

//Main game loop
void Core::coreLoop() {
    renderEngine = new RenderEngine(properties.window, properties.screenWidth, properties.screenHeight);
    audioEngine = new AudioEngine;
    logic = new Logic;

	glfwSetKeyCallback(properties.window, windowKeyInput);
	glfwSetInputMode(properties.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // initialize audio engine
    audioObserver = new AudioObserver();
    audioEngine->init();
    audioEngine->bindObserver(audioObserver);
    audioEngine->loadSound(AudioPaths::getInstance()->bgm1, false, true, true); // load music
    audioEngine->setSoundVol(0.5);
    audioEvents = new AudioObservable();
    audioEvents->addAudioObserver(audioObserver);
    EntityManager::getInstance()->bindAudioObservable(audioEvents);
    // load common sounds
    audioEngine->loadSound(AudioPaths::getInstance()->engineIdle, true, false, false);
    audioEngine->loadSound(AudioPaths::getInstance()->engineRev, true, false, false);
    audioEngine->loadSound(AudioPaths::getInstance()->rifleShot, true, false, false);

    // play music
    int musicChannel = audioEngine->playSounds(Core::dataPath + "sound/bgm1.mp3", glm::vec3(0, 0, 0), 1);
    audioEngine->setChannelVolume(musicChannel, -25.f);
    // end audio init

    PhysicsEngine::getInstance()->initPhysics();
    PhysicsEngine::getInstance()->bindAudioObservable(audioEvents);
    Camera camera(properties.screenWidth,properties.screenHeight);
	cameras.push_back(&camera);

    logic->bindCamera(&camera);
    logic->bindAudioObservable(audioEvents);

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

        // messy audio update function begins here
        audioEngine->updateListenerPos(camera.getPosition(), camera.getForward(), camera.getUp(), timeDiff);
        audioEngine->update3dListener();
        if (!audioEngine->audioObserver->isEmpty()) {
            AudioEvent nextAudio = audioEngine->audioObserver->nextAudioEvent();
            audioEngine->playSounds(nextAudio.soundfile, nextAudio.position, audioEngine->soundVol);
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
Entity* Core::playerVehicle = nullptr;
Entity* playerTarget = nullptr;
int playerReticleId = -1;

int physxIterCounterId = -1;
int mainfpsCounterId = -1;
int playerHealthId = -1;
int playerResourceId = -1;
int enemyCountId = -1;
int healthBarGreenId = -1;
int healthBarRedId = -1;
bool controllerButtonPressed = false;
#define		GAMEPAD_A	0
#define     GAMEPAD_X	2
#define     GAMEPAD_START	7

bool replayUIShown = false;
bool replayUIInitialized = false;
bool inPauseMenu = false;

std::vector<int> currentImageUiIds;

void Core::runGame() {
    if (properties.isGameInitialized == false) {
        // We need to clear the menu and all of its buttons and spawn the UI/Game elements for our scene
        renderEngine->ui->clearAllUI();
        EntityManager::getInstance()->destroyAllEntities();

        physxIterCounterId = renderEngine->ui->addText("78", 5, 5, 0.5, glm::vec3(0, 1, 0));
        mainfpsCounterId = renderEngine->ui->addText("100", 50, 5, 0.5, glm::vec3(1, 1, 0));

		Core::upgradeLizardId = renderEngine->ui->addImageDiffSize(*TextureDataManager::getImageData("upgradeAvailable.png"), float(*properties.screenWidth - 300), 315, 0);
		healthBarGreenId = renderEngine->ui->addImageDiffSize(*TextureDataManager::getImageData("healthGreen.png"), 118, 77, 1);
		healthBarRedId = renderEngine->ui->addImageDiffSize(*TextureDataManager::getImageData("healthRed.png"), 115, 73, 1);
		renderEngine->ui->addImage(*TextureDataManager::getImageData("UITopRight.png"), float(*properties.screenWidth-326), 0, 1);
		renderEngine->ui->addImage(*TextureDataManager::getImageData("UITopLeft.png"), 0, 0, 1);

        playerReticleId = renderEngine->ui->addImage(*TextureDataManager::getImageData("reticle3.png"),0, 0);
		playerTarget = nullptr;

        // Health and resource text
        playerHealthId = renderEngine->ui->addText("health", 40, 40, 1, glm::vec3(0.5, 1, 0));
        playerResourceId = renderEngine->ui->addText("resources", float(*properties.screenWidth - 160), 120, 0.5, glm::vec3(0.8, 1, 1));
		enemyCountId = renderEngine->ui->addText("Enemy Count: 9", float(*properties.screenWidth - 140), 230, 0.5, glm::vec3(1, 0.8, 0));

        WorldGenerator::getInstance()->generateWorld();
        playerVehicle = EntityManager::getInstance()->getVehicleEntities().at(0);
        properties.isGameInitialized = true;

        // Last thing to do is correctly set the time
        realtimeSinceStartup = glfwGetTime();
        physicsTime = realtimeSinceStartup;
    }

    // ***************************************** START OF GAME SPECIFIC CODE *******************************************************
    HealthComponent* playerHealthComp = static_cast<HealthComponent*>(playerVehicle->getComponent(HEALTH));
    const float playerHealth = playerHealthComp->getCurrentHealth();
	const float playerMaxHealth = playerHealthComp->getMaxHealth();
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
		} else if (buttons[GAMEPAD_X]) {
			if (!controllerButtonPressed) {
				upgradeButtonPressed = true;
			}
			controllerButtonPressed = true;
		} else if (buttons[GAMEPAD_A]) {
			if (!controllerButtonPressed) {
				changeTargetPressed = true;
			}
			controllerButtonPressed = true;
		} else {
            controllerButtonPressed = false;
        }
    }
	UpgradeComponent* playerUC = static_cast<UpgradeComponent*>(playerVehicle->getComponent(UPGRADE));
    
	std::ostringstream oss;
    //oss << "Health: " << round(playerHealth);
    std::string playerHealthStr = oss.str();
	float redScale = 0.5 + 1 * (playerUC->getChassisLevel() * 0.25);
	float greenScale = playerHealth / playerMaxHealth * redScale;
	renderEngine->ui->modifyImageDiffSize(healthBarGreenId, nullptr, nullptr, &greenScale);
	renderEngine->ui->modifyImageDiffSize(healthBarRedId, nullptr, nullptr, &redScale);
    renderEngine->ui->modifyText(playerHealthId, &playerHealthStr, nullptr, nullptr, nullptr, nullptr);
    bool isPlayerDead = playerHealthComp->isDead();

	
    // Only set this value to false if the game has yet to be paused. Also used for unpausing.
    if (pauseButtonPressed && properties.isPaused == false) {
        if (!replayUIShown) { // only allow pausing if the game is not over
            properties.isPaused = true;
			inPauseMenu = true;
        }
        pauseButtonPressed = false;
	} else if (upgradeButtonPressed && properties.isInUpgradeMenu == false) {
		if (!replayUIShown && !inPauseMenu) {
			properties.isInUpgradeMenu = true;
			properties.isPaused = true;
		}
        upgradeButtonPressed = false;
	}

	// for button placement
	int buttonTop = 200;
	int buttonTopHeight = 75;

    if (replayUIShown == false && isPlayerDead) { // Defeat!
        //renderEngine->ui->addText("DEFEAT!", 100, 150, 4, glm::vec3(1, 0, 0));
		currentImageUiIds.push_back(renderEngine->ui->addImage(*TextureDataManager::getImageData("buttonLongDefeatSign.png"), 0, buttonTop + buttonTopHeight));

        replayUIShown = true;
        replayUIInitialized = false;
    }
    else if (replayUIShown == false && EntityManager::getInstance()->getVehicleEntities().size() == 1) { // Victory!
        //renderEngine->ui->addText("VICTORY!", 100, 150, 4, glm::vec3(0, 1, 0));
		currentImageUiIds.push_back(renderEngine->ui->addImage(*TextureDataManager::getImageData("buttonLongVictorySign.png"), 0, buttonTop + buttonTopHeight));

        replayUIShown = true;
        replayUIInitialized = false;
    }

    if (replayUIShown) {
        runEndGameMenu();
    }
    // Now we need to update the resources collected
    oss.str("");
    oss.clear();
    oss << roundf(playerUC->getCurrentResources()) << "/" << roundf(playerUC->getMaxResources());
    std::string playerResources = oss.str();
    renderEngine->ui->modifyText(playerResourceId, &playerResources, nullptr, nullptr, nullptr, nullptr);

	// enemy count
	oss.str("");
	oss.clear();
	oss << EntityManager::getInstance()->getVehicleEntities().size() - 1;
	std::string enemyCount = oss.str();
	renderEngine->ui->modifyText(enemyCountId, &enemyCount, nullptr, nullptr, nullptr, nullptr);

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



    //We could make a pause game feature by just rendering stuff and disabling all
    // the other stuff... although feel free to change this if you think some other
    // approach is better
	if (properties.isPaused) {
        float loc = -100.f;
        float scale = 1.f;
        renderEngine->ui->modifyImageDiffSize(playerReticleId, &loc, &loc, &scale, &scale);
		if (properties.isInUpgradeMenu) {
			runUpgradeMenu();
		} else {
			runPauseMenu();
		}
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

        ComponentManager::getInstance()->updateWeaponAiming();
        for (unsigned int i = 1; i < EntityManager::getInstance()->getVehicleEntities().size(); i++) {
            logic->finiteStateMachine(EntityManager::getInstance()->getVehicleEntities().at(i));
        }

        WorldGenerator::getInstance()->regenerateCrystal();

        // Render all of the renderer components here
        ComponentManager::getInstance()->performPhysicsLogic();
        ComponentManager::getInstance()->performProjectileLogic();
        ComponentManager::getInstance()->performRendering();

        if (forceReplay) {
            printf("Forcing game reset!\n");
            forceReplay = false;
            properties.isGameInitialized = false;
        }

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
			offset =  glm::vec3(offset.x, 0, offset.z); // Prevent the camera from doing weird stuff

            float dotProd = glm::dot(velocity, playerVehicle->getForwardVector());

            // Makes the cmaera look behind the car at certain speeds.
			// TODO: Make right bumper be look backward (Also middle mouse button?)
            /*if (dotProd < -30.0f && static_cast<DriveComponent*>(playerVehicle->getComponent(DRIVE))->getBrake()) {
            movingForward = false;
            } else if (dotProd > 5.f) {
            movingForward = true;
            }*/

            if (!movingForward) offset = -offset;

            const float chassisLevel = static_cast<UpgradeComponent*>(playerVehicle->getComponent(UPGRADE))->getChassisLevel();

			// Check if the player wants to update the target
			if (changeTargetPressed) { // grab the target that is closest to being in front of the player
				playerTarget = nullptr;
				float smallestAngle = 10.f;
				for (int i = 0; i < EntityManager::getInstance()->getVehicleEntities().size(); i++) {
					glm::vec3 toTarget = normalize(EntityManager::getInstance()->getVehicleEntities().at(i)->getPosition() - playerVehicle->getPosition());
					float angle = glm::orientedAngle(toTarget, playerVehicle->getForwardVector(), glm::vec3(0,1,0));
					if (abs(angle) < smallestAngle) {
						smallestAngle = abs(angle);
						playerTarget = EntityManager::getInstance()->getVehicleEntities().at(i);
					}
				}
				changeTargetPressed = false;
				static_cast<WeaponComponent*>(playerVehicle->getComponent(WEAPON))->updateTarget(playerTarget);
			} else if (playerTarget != nullptr) { // make sure our vehicle is still alive
				auto it = EntityManager::getInstance()->getVehicleEntities().begin();
				bool found = false;
				for (int i = 0; i < EntityManager::getInstance()->getVehicleEntities().size(); i++) {
				    if (EntityManager::getInstance()->getVehicleEntities().at(i) == playerTarget) {
				        found = true;
                        break;
				    }
				}
				if (!found) playerTarget = nullptr;
			}
			if (playerTarget != nullptr) {
				glm::vec2 screenPos = cameras[0]->worldToScreenPoint(playerTarget->getPosition());
				// adjust the scale accordingly based on the size and the distance to the target
				float multi = 1.f;
				float distanceMulti = glm::distance(playerTarget->getPosition(), playerVehicle->getPosition()) / 100.f;
				if (distanceMulti < 1.f) distanceMulti += (1.f - distanceMulti) / 2.f; // Make the close shape not quite so big
				multi /= distanceMulti;

				GLfloat x = screenPos.x - (50.f * multi);
				GLfloat y = screenPos.y - (50.f * multi);

				renderEngine->ui->modifyImageDiffSize(playerReticleId, &x, &y, &multi, &multi);
			} else { // No target, move reticle offscreen
				GLfloat x = -200.f;
				GLfloat y = -200.f;
				GLfloat m = 1.f;
				renderEngine->ui->modifyImageDiffSize(playerReticleId, &(x), &(y), &m, &m);
			}

            cameras[0]->rotateCameraTowardPoint(playerVehicle->getPosition() + offset * 10.0f, 7.5f * fixedStepTimediff);
            cameras[0]->lerpCameraTowardPoint(playerVehicle->getPosition() + offset * -12.0f * chassisLevel + glm::vec3(0, 8 + 4.f * (chassisLevel - 0.5f), 0), 7.5f * fixedStepTimediff);
        }
        ComponentManager::getInstance()->performFloatingTextLogic();
    }
}

// **************************** Variables for menu ********************************
// i moved up image uiid to line 217 :')
std::vector<int> currentTextUiIds;

std::vector<int> constantTextUI;
std::vector<int> constantImageUI;
Entity* mainMenuEnt = nullptr;

enum MainMenuState {NONE, MAINMENU, OPTIONS, GAMEMODES, CONTROLS};
MainMenuState currentMainMenuState = NONE;
MainMenuState nextMainMenuState = MAINMENU;
int currentChoiceIndex = 0;
int maxChoiceIndex = 1;

#define     GAMEPAD_A		0
#define 	GAMEPAD_DPAD_UP   10
#define 	GAMEPAD_DPAD_DOWN   12

#define     GAMEPAD_LEFT_JOYSTICK_Y 1

int menuLightId1;
int menuLightId2;

bool previouslyPressed = false;

void Core::runMenu() {

    if (properties.isGameInitialized) {
        // If the game has been initialized, then we need to completely clear it before we do anything else.
        renderEngine->ui->clearAllUI();
        EntityManager::getInstance()->destroyAllEntities();

        currentMainMenuState = NONE;
        nextMainMenuState = MAINMENU;

        //renderEngine->ui->addText("KARSIO", 155, 200, 2, glm::vec3(0.7, 0.7, 0), 1);
        currentChoiceIndex = 0;

        menuLightId1 = renderEngine->world->getNextAvailableLightID();
        menuLightId2 = renderEngine->world->getNextAvailableLightID();

        renderEngine->world->setLight(menuLightId1, glm::vec3(15, 0, 0), glm::vec3(240, 240, 300));
        renderEngine->world->setLight(menuLightId2, glm::vec3(5, 0, 10), glm::vec3(240, 240, 300));


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

		// for button placement
		int buttonTop = 10;
		int buttonTopHeight = 73;
		int buttonHeight = 169;
		int titleHeight = 250;

        switch (currentMainMenuState) {
        case OPTIONS:
			currentImageUiIds.push_back(renderEngine->ui->addImage(*TextureDataManager::getImageData("buttonTopCap.png"), 0, buttonTop));
			currentImageUiIds.push_back(renderEngine->ui->addImage(*TextureDataManager::getImageData("buttonLongTitleSign.png"), 0, buttonTop + buttonTopHeight));
			currentImageUiIds.push_back(renderEngine->ui->addImage(*TextureDataManager::getImageData("pauseMenu.png"), 0, buttonTop + buttonTopHeight + titleHeight));
			currentImageUiIds.push_back(renderEngine->ui->addImage(*TextureDataManager::getImageData("buttonExtend2.png"), 0, buttonTop + buttonTopHeight + titleHeight + buttonHeight * 2));
			currentImageUiIds.push_back(renderEngine->ui->addImage(*TextureDataManager::getImageData("button.png"), 0, buttonTop + buttonTopHeight + titleHeight + buttonHeight * 3));
			currentImageUiIds.push_back(renderEngine->ui->addImage(*TextureDataManager::getImageData("buttonEndCapShort.png"), 0, buttonTop + buttonTopHeight + titleHeight + buttonHeight * 4 - 1));

			currentTextUiIds.push_back(renderEngine->ui->addText("Back", 155, 190 + buttonHeight * 4, 1, menuBaseTextColor, 1));
			currentTextUiIds.push_back(renderEngine->ui->addText("Options are TBD.", 155, 190 + buttonHeight, 1, menuBaseTextColor, 1));

			maxChoiceIndex = 1;
			break;
		case CONTROLS:
			currentImageUiIds.push_back(renderEngine->ui->addImage(*TextureDataManager::getImageData("controller.png"), 650, 250, 1));
			
			currentImageUiIds.push_back(renderEngine->ui->addImage(*TextureDataManager::getImageData("buttonTopCap.png"), 0, buttonTop));
			currentImageUiIds.push_back(renderEngine->ui->addImage(*TextureDataManager::getImageData("buttonLongTitleSign.png"), 0, buttonTop + buttonTopHeight));
			currentImageUiIds.push_back(renderEngine->ui->addImage(*TextureDataManager::getImageData("buttonExtend2.png"), 0, buttonTop + buttonTopHeight + titleHeight));
			currentImageUiIds.push_back(renderEngine->ui->addImage(*TextureDataManager::getImageData("buttonExtend.png"), 0, buttonTop + buttonTopHeight + titleHeight + buttonHeight));
			currentImageUiIds.push_back(renderEngine->ui->addImage(*TextureDataManager::getImageData("buttonExtend2.png"), 0, buttonTop + buttonTopHeight + titleHeight + buttonHeight * 2));
			currentImageUiIds.push_back(renderEngine->ui->addImage(*TextureDataManager::getImageData("button.png"), 0, buttonTop + buttonTopHeight + titleHeight + buttonHeight * 3));
			currentImageUiIds.push_back(renderEngine->ui->addImage(*TextureDataManager::getImageData("buttonEndCapShort.png"), 0, buttonTop + buttonTopHeight + titleHeight + buttonHeight * 4 - 1));

			currentTextUiIds.push_back(renderEngine->ui->addText("Back", 155, 190 + buttonHeight * 4, 1, menuBaseTextColor, 1));
			
            maxChoiceIndex = 1;
            break;
        case GAMEMODES:
			currentImageUiIds.push_back(renderEngine->ui->addImage(*TextureDataManager::getImageData("buttonTopCap.png"), 0, buttonTop));
			currentImageUiIds.push_back(renderEngine->ui->addImage(*TextureDataManager::getImageData("buttonLongTitleSign.png"), 0, buttonTop + buttonTopHeight));
			currentImageUiIds.push_back(renderEngine->ui->addImage(*TextureDataManager::getImageData("buttonLong.png"), 0, buttonTop + buttonTopHeight + titleHeight));
			currentImageUiIds.push_back(renderEngine->ui->addImage(*TextureDataManager::getImageData("buttonExtend.png"), 0, buttonTop + buttonTopHeight + titleHeight + buttonHeight));
			currentImageUiIds.push_back(renderEngine->ui->addImage(*TextureDataManager::getImageData("buttonExtend2.png"), 0, buttonTop + buttonTopHeight + titleHeight + buttonHeight * 2));
			currentImageUiIds.push_back(renderEngine->ui->addImage(*TextureDataManager::getImageData("button.png"), 0, buttonTop + buttonTopHeight + titleHeight + buttonHeight * 3));
			currentImageUiIds.push_back(renderEngine->ui->addImage(*TextureDataManager::getImageData("buttonEndCapShort.png"), 0, buttonTop + buttonTopHeight + titleHeight + buttonHeight * 4 - 1));


            currentTextUiIds.push_back(renderEngine->ui->addText("Last Kar Driving", 155, 190 + buttonHeight, 1, menuSelectedTextColor, 1));
            currentTextUiIds.push_back(renderEngine->ui->addText("Back", 155, 190 + buttonHeight * 4, 1, menuBaseTextColor, 1));
            maxChoiceIndex = 2;
            break;
        default: // Default is the MAINMENU			
			
			currentImageUiIds.push_back(renderEngine->ui->addImage(*TextureDataManager::getImageData("buttonTopCap.png"), 0, buttonTop));
			currentImageUiIds.push_back(renderEngine->ui->addImage(*TextureDataManager::getImageData("buttonLongTitleSign.png"), 0, buttonTop + buttonTopHeight));
			currentImageUiIds.push_back(renderEngine->ui->addImage(*TextureDataManager::getImageData("button.png"), 0, buttonTop + buttonTopHeight + titleHeight));
			currentImageUiIds.push_back(renderEngine->ui->addImage(*TextureDataManager::getImageData("button2.png"), 0, buttonTop + buttonTopHeight + titleHeight + buttonHeight));
			currentImageUiIds.push_back(renderEngine->ui->addImage(*TextureDataManager::getImageData("button.png"), 0, buttonTop + buttonTopHeight + titleHeight + buttonHeight*2));
			currentImageUiIds.push_back(renderEngine->ui->addImage(*TextureDataManager::getImageData("button.png"), 0, buttonTop + buttonTopHeight + titleHeight + buttonHeight*3));
			currentImageUiIds.push_back(renderEngine->ui->addImage(*TextureDataManager::getImageData("buttonEndCapShort.png"), 0, buttonTop + buttonTopHeight + titleHeight + buttonHeight * 4 - 1));
			   
			currentTextUiIds.push_back(renderEngine->ui->addText("Play Game", 155, 190 + buttonHeight, 1, glm::vec3(0, 1, 0), 1));
			currentTextUiIds.push_back(renderEngine->ui->addText("Options", 155, 190 + buttonHeight*2, 1, glm::vec3(1, 1, 0), 1));
			currentTextUiIds.push_back(renderEngine->ui->addText("Controls", 155, 190 + buttonHeight*3, 1, glm::vec3(1, 1, 0), 1));
			currentTextUiIds.push_back(renderEngine->ui->addText("Exit", 155, 190 + buttonHeight*4, 1, glm::vec3(1, 1, 0), 1));

            maxChoiceIndex = 4;
            break;
        }
    }

    // Process user input to change the selection: 
    // CONTROLLER IMPLEMENTATION
    int present = glfwJoystickPresent(GLFW_JOYSTICK_1);
    if (present) {
        int buttonCount, axesCount;
        const float *axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axesCount);
        const unsigned char *buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);
        if (buttons[GAMEPAD_DPAD_DOWN] || axes[GAMEPAD_LEFT_JOYSTICK_Y] < -0.5f) {
            if (!previouslyPressed) {
                currentChoiceIndex = (currentChoiceIndex + 1) % maxChoiceIndex;
            }
            previouslyPressed = true;
        } else if (buttons[GAMEPAD_DPAD_UP] || axes[GAMEPAD_LEFT_JOYSTICK_Y] > 0.5f) {
            if (!previouslyPressed) {
                currentChoiceIndex = currentChoiceIndex == 0 ? maxChoiceIndex - 1: currentChoiceIndex - 1;
            }
            previouslyPressed = true;
        } else if (buttons[GAMEPAD_A]) {
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
					nextMainMenuState = CONTROLS;
                } else if (currentChoiceIndex == 3) {
                    glfwSetWindowShouldClose(globalWindow, GL_TRUE);
                    properties.isRunning = false;
                }
            break;
            case OPTIONS:
                if (currentChoiceIndex == 0) {
                    nextMainMenuState = MAINMENU;
                }
            break;
			case CONTROLS:
				if (currentChoiceIndex == 0) {
					nextMainMenuState = MAINMENU;
				}
				break;
            case GAMEMODES:
                if (currentChoiceIndex == 0) {
                    // Time to launch the actual game
                    renderEngine->world->freeLightWithID(menuLightId1);
                    renderEngine->world->freeLightWithID(menuLightId2);
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
            renderEngine->ui->modifyText(currentTextUiIds[i],nullptr, nullptr, nullptr, nullptr, &menuSelectedTextColor, nullptr);
        } else {
            renderEngine->ui->modifyText(currentTextUiIds[i],nullptr, nullptr, nullptr, nullptr, &menuBaseTextColor, nullptr);
        }
    }

    // Make the camera look at the vehicle
    cameras[0]->rotateCameraTowardPoint(mainMenuEnt->getPosition(), 5.f * timeDiff);
    cameras[0]->lerpCameraTowardPoint(glm::vec3(-20, 5, 0), 5.0f * timeDiff);

    // We need to render the UI if this is the case and keep track of where we are/what is selected.
    UpgradeComponent* uc = static_cast<UpgradeComponent*>(mainMenuEnt->getComponent(UPGRADE));
    uc->addResources(timeDiff * 5.f);

    if (uc->isUpgradeAvailable()) {
        UpgradeType type = UpgradeType(logic->randomNum(0, 3));
        while (!uc->canUpgradeType(type)) {
            type = UpgradeType(logic->randomNum(0, 3));
        }
        uc->upgradeVehicle(type);
    }

    static_cast<PhysicsComponent*>(mainMenuEnt->getComponent(PHYSICS))->getRigidBody()->setGlobalPose(physx::PxTransform(physx::PxVec3(5, 0, 0),physx::PxQuat(realtimeSinceStartup, physx::PxVec3(0,1,0))));

    // Update the physics and rendering aspects of the simulation
    PhysicsEngine::getInstance()->simulateTimeInSeconds(0.01f);
    ComponentManager::getInstance()->performPhysicsLogic();
    ComponentManager::getInstance()->performRendering();
}

enum PauseMenuState { PAUSENONE, PAUSEMAIN, PAUSEOPTIONS };
PauseMenuState currentPauseMenuState = PAUSENONE;
PauseMenuState nextPauseMenuState = PAUSEMAIN;

void Core::runPauseMenu() {

    if (!properties.isIngameMenuInitialized) {

        currentPauseMenuState = PAUSENONE;
        nextPauseMenuState = PAUSEMAIN;
        currentChoiceIndex = 0;

        //constantTextUI.push_back(renderEngine->ui->addText("Game Paused", 155, 300, 1.25, glm::vec3(1, 1, 0), 1));
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

		// for button placement
		int buttonTop = 200;
		int buttonTopHeight = 75;
		int buttonHeight = 169;

        switch (currentPauseMenuState) {
        case PAUSEOPTIONS:

			currentImageUiIds.push_back(renderEngine->ui->addImage(*TextureDataManager::getImageData("buttonTopCap.png"), 0, buttonTop));
			currentImageUiIds.push_back(renderEngine->ui->addImage(*TextureDataManager::getImageData("buttonLongPauseSign.png"), 0, buttonTop + buttonTopHeight));
			currentImageUiIds.push_back(renderEngine->ui->addImage(*TextureDataManager::getImageData("pauseMenu.png"), 0, buttonTop + buttonTopHeight + buttonHeight));
			currentImageUiIds.push_back(renderEngine->ui->addImage(*TextureDataManager::getImageData("button.png"), 0, buttonTop + buttonTopHeight + buttonHeight * 3));
			currentImageUiIds.push_back(renderEngine->ui->addImage(*TextureDataManager::getImageData("buttonEndCapShort.png"), 0, buttonTop + buttonTopHeight + buttonHeight * 4 - 1));

			currentTextUiIds.push_back(renderEngine->ui->addText("Back", 155, 300 + buttonHeight * 3, 1, menuSelectedTextColor, 1));
			currentTextUiIds.push_back(renderEngine->ui->addText("Options are TBD.", 155, 300 + buttonHeight, 1, menuBaseTextColor, 1));

            maxChoiceIndex = 1;
            break;
        default: // Default is the PAUSEMENU

			currentImageUiIds.push_back(renderEngine->ui->addImage(*TextureDataManager::getImageData("buttonTopCap.png"), 0, buttonTop));
			currentImageUiIds.push_back(renderEngine->ui->addImage(*TextureDataManager::getImageData("buttonLongPauseSign.png"), 0, buttonTop + buttonTopHeight));
			currentImageUiIds.push_back(renderEngine->ui->addImage(*TextureDataManager::getImageData("button.png"), 0, buttonTop + buttonTopHeight + buttonHeight));
			currentImageUiIds.push_back(renderEngine->ui->addImage(*TextureDataManager::getImageData("button2.png"), 0, buttonTop + buttonTopHeight + buttonHeight * 2));
			currentImageUiIds.push_back(renderEngine->ui->addImage(*TextureDataManager::getImageData("button.png"), 0, buttonTop + buttonTopHeight + buttonHeight * 3));
			currentImageUiIds.push_back(renderEngine->ui->addImage(*TextureDataManager::getImageData("buttonEndCapShort.png"), 0, buttonTop + buttonTopHeight + buttonHeight * 4 - 1));


            currentTextUiIds.push_back(renderEngine->ui->addText("Resume", 155, 300 + buttonHeight, 1, menuSelectedTextColor, 1));
            currentTextUiIds.push_back(renderEngine->ui->addText("Options", 155, 300 + buttonHeight * 2, 1, menuBaseTextColor, 1));
            currentTextUiIds.push_back(renderEngine->ui->addText("Main Menu", 155, 300 + buttonHeight * 3, 1, menuBaseTextColor, 1));
            maxChoiceIndex = 3;
            break;
        }
    }

    // Process user input to change the selection: 
    // CONTROLLER IMPLEMENTATION
    int present = glfwJoystickPresent(GLFW_JOYSTICK_1);
    if (present) {
        int buttonCount, axesCount;
        const float *axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axesCount);
        const unsigned char *buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);
        if (buttons[GAMEPAD_DPAD_DOWN] || axes[GAMEPAD_LEFT_JOYSTICK_Y] < -0.5f) {
            if (!previouslyPressed) {
                currentChoiceIndex = (currentChoiceIndex + 1) % maxChoiceIndex;
            }
            previouslyPressed = true;
        }
        else if (buttons[GAMEPAD_DPAD_UP] || axes[GAMEPAD_LEFT_JOYSTICK_Y] > 0.5f) {
            if (!previouslyPressed) {
                currentChoiceIndex = currentChoiceIndex == 0 ? maxChoiceIndex - 1 : currentChoiceIndex - 1;
            }
            previouslyPressed = true;
        }
        else if (buttons[GAMEPAD_A]) {
            if (!previouslyPressed) {
                enterPressed = true;
            }
            previouslyPressed = true;
        }
        else {
            previouslyPressed = false;
        }
    }
    else { // KEYBOARD IMPLEMENTATION
        if (keyPressedUp) {
            currentChoiceIndex = currentChoiceIndex == 0 ? maxChoiceIndex - 1 : currentChoiceIndex - 1;
            keyPressedUp = false;
        }
        else if (keyPressedDown) {
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
				inPauseMenu = false;

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

            }
            else if (currentChoiceIndex == 1) {
                nextPauseMenuState = PAUSEOPTIONS;
            }
            else if (currentChoiceIndex == 2) {
                properties.isPaused = false;
				inPauseMenu = false;
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
            renderEngine->ui->modifyText(currentTextUiIds[i], nullptr, nullptr, nullptr, nullptr, &menuSelectedTextColor, nullptr);
        }
        else {
            renderEngine->ui->modifyText(currentTextUiIds[i], nullptr, nullptr, nullptr, nullptr, &menuBaseTextColor, nullptr);
        }
    }
}

void Core::runEndGameMenu() {

    if (replayUIInitialized == false) {

        currentImageUiIds.clear();
        currentTextUiIds.clear();

		// for button placement
		int buttonTop = 200;
		int buttonTopHeight = 75;
		int buttonHeight = 169;

		currentImageUiIds.push_back(renderEngine->ui->addImage(*TextureDataManager::getImageData("buttonTopCap.png"), 0, buttonTop));
		currentImageUiIds.push_back(renderEngine->ui->addImage(*TextureDataManager::getImageData("button.png"), 0, buttonTop + buttonTopHeight + buttonHeight));
		currentImageUiIds.push_back(renderEngine->ui->addImage(*TextureDataManager::getImageData("buttonExtend.png"), 0, buttonTop + buttonTopHeight + buttonHeight * 2));
		currentImageUiIds.push_back(renderEngine->ui->addImage(*TextureDataManager::getImageData("button.png"), 0, buttonTop + buttonTopHeight + buttonHeight * 3));
		currentImageUiIds.push_back(renderEngine->ui->addImage(*TextureDataManager::getImageData("buttonEndCapShort.png"), 0, buttonTop + buttonTopHeight + buttonHeight * 4 - 1));

		currentTextUiIds.push_back(renderEngine->ui->addText("Play Again", 155, 300 + buttonHeight, 1, menuSelectedTextColor, 1));
		currentTextUiIds.push_back(renderEngine->ui->addText("Main Menu", 155, 300 + buttonHeight * 3, 1, menuBaseTextColor, 1));

        currentChoiceIndex = 0;
        maxChoiceIndex = 2;
        replayUIInitialized = true;
    }

    int present = glfwJoystickPresent(GLFW_JOYSTICK_1);
    if (present) {
        int buttonCount, axesCount;
        const float *axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axesCount);
        const unsigned char *buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);
        if (buttons[GAMEPAD_DPAD_DOWN] || axes[GAMEPAD_LEFT_JOYSTICK_Y] < -0.5f) {
            if (!previouslyPressed) {
                currentChoiceIndex = (currentChoiceIndex + 1) % maxChoiceIndex;
            }
            previouslyPressed = true;
        }
        else if (buttons[GAMEPAD_DPAD_UP] || axes[GAMEPAD_LEFT_JOYSTICK_Y] > 0.5f) {
            if (!previouslyPressed) {
                currentChoiceIndex = currentChoiceIndex == 0 ? maxChoiceIndex - 1 : currentChoiceIndex - 1;
            }
            previouslyPressed = true;
        }
        else if (buttons[GAMEPAD_A]) {
            if (!previouslyPressed) {
                enterPressed = true;
            }
            previouslyPressed = true;
        }
        else {
            previouslyPressed = false;
        }
    }
    else { // KEYBOARD IMPLEMENTATION
        if (keyPressedUp) {
            currentChoiceIndex = currentChoiceIndex == 0 ? maxChoiceIndex - 1 : currentChoiceIndex - 1;
            keyPressedUp = false;
        }
        else if (keyPressedDown) {
            currentChoiceIndex = (currentChoiceIndex + 1) % maxChoiceIndex;
            keyPressedDown = false;
        }
    }

    if (enterPressed) {
        enterPressed = false;
        switch (currentChoiceIndex) {
        case 0:
            // Replay
            replayUIInitialized = false;
            replayUIShown = false;
            properties.isGameInitialized = false;
            break;
        default:
            // Default is to go back to the main menu.
            replayUIInitialized = false;
            replayUIShown = false;
            properties.inMainMenu = true;
            break;
        }
    }

    // Make the current selection green
    for (int i = 0; i < currentTextUiIds.size(); ++i) {
        if (i == currentChoiceIndex) {
            renderEngine->ui->modifyText(currentTextUiIds[i], nullptr, nullptr, nullptr, nullptr, &menuSelectedTextColor, nullptr);
        }
        else {
            renderEngine->ui->modifyText(currentTextUiIds[i], nullptr, nullptr, nullptr, nullptr, &menuBaseTextColor, nullptr);
        }
    }

}

bool upgradeMenuInitialized = false;
std::vector<UpgradeType> upgrades;
float selectPanelX = 1075;
float selectPanelY = 115;
float statPanelX = 660;
float statPanelY = 112;

void Core::runUpgradeMenu() {
	
	glm::vec3 selectionDisabledColour = glm::vec3(0.25, 0.2, 0.22);
	glm::vec3 selectionColour = glm::vec3(0.26, 0.18, 0.28);

	glm::vec3 generalTextColour = glm::vec3(0.16, 0.13, 0.33);
	glm::vec3 unavailableTextColour = glm::vec3(0.29, 0.04, 0.04);
	glm::vec3 availableTextColour = glm::vec3(0.04, 0.25, 0.11);

	// Grab the player upgrade component first
	UpgradeComponent* uc = static_cast<UpgradeComponent*>(playerVehicle->getComponent(UPGRADE));

	if (upgradeMenuInitialized == false) {

		currentImageUiIds.clear();
		currentTextUiIds.clear();

		int x, y;
		y = 400;
		x = 160;
		float vehicleScale = 0.25;
		// show preview image of player
		std::string cLevel = std::to_string(uc->getChassisLevel());
		std::string aLevel = std::to_string(uc->getArmorLevel());
		std::string rLevel = std::to_string(uc->getRamLevel());
		std::string gLevel = std::to_string(uc->getGunLevel());

		if (uc->getChassisLevel() == 1) {
			vehicleScale = 0.25;
			x = 130;
			y = 400;
		}
		else if (uc->getChassisLevel() == 2) {
			vehicleScale = 0.29;
			x = 80;
			y = 370;
		}
		else {
			vehicleScale = 0.33;
			x = 50;
			y = 340;
		}
		
		currentImageUiIds.push_back(renderEngine->ui->addImage(*TextureDataManager::getImageData("previewGun" + gLevel + ".png"), x, y, vehicleScale));
		currentImageUiIds.push_back(renderEngine->ui->addImage(*TextureDataManager::getImageData("previewRam" + rLevel + ".png"), x, y, vehicleScale));
		currentImageUiIds.push_back(renderEngine->ui->addImage(*TextureDataManager::getImageData("previewChassis" + cLevel + "A" + aLevel + ".png"), x, y, vehicleScale));

        if (!uc->isUpgradeAvailable()) {
            currentImageUiIds.push_back(Core::renderEngine->ui->addImageDiffSize(*TextureDataManager::getImageData("upgradeMenuNotEnough.png"), 600, 0));
        }
		else {
			currentImageUiIds.push_back(renderEngine->ui->addImage(*TextureDataManager::getImageData("upgradeMenu.png"), 600, 0));
		}
		currentImageUiIds.push_back(renderEngine->ui->addImage(*TextureDataManager::getImageData("upgradePreview.png"), 0, 20));

		
		maxChoiceIndex = 0;
		upgrades.clear();

		// Check if chassis should be active
		if (uc->canUpgradeType(CHASSIS_UPGRADE)) {
			upgrades.push_back(CHASSIS_UPGRADE);
			currentTextUiIds.push_back(renderEngine->ui->addText("Chassis", selectPanelX + 90, 300, 1, generalTextColour, 1));
			constantTextUI.push_back(Core::renderEngine->ui->addText("Upgrade Available", statPanelX + 50, 340, 0.5, availableTextColour, 1));
		} else {
			constantTextUI.push_back(renderEngine->ui->addText("Chassis", selectPanelX + 90, 300, 1, selectionDisabledColour, 1));
			if (uc->getChassisLevel() < 3) {
				std::ostringstream oss;
				oss << "Total lvl " << (uc->getNextUpgradeBoundary() + 4) << " Required";
				std::string requiredTotalLevel = oss.str();
				constantTextUI.push_back(Core::renderEngine->ui->addText(requiredTotalLevel, statPanelX + 50, 340, 0.5, unavailableTextColour, 1));
				oss.str("");
				oss.clear();
			} else {
				constantTextUI.push_back(Core::renderEngine->ui->addText("Fully Upgraded", statPanelX + 50, 340, 0.5, generalTextColour, 1));
			}
		}

		// Check if gun should be active
		if (uc->canUpgradeType(GUN_UPGRADE)) {
			upgrades.push_back(GUN_UPGRADE);
			currentTextUiIds.push_back(renderEngine->ui->addText("Gun", selectPanelX + 90, 300 + selectPanelY, 1, generalTextColour, 1));
			constantTextUI.push_back(Core::renderEngine->ui->addText("Upgrade Available", statPanelX + 50, 340 + statPanelY, 0.5, availableTextColour, 1));
		} else {
			constantTextUI.push_back(renderEngine->ui->addText("Gun", selectPanelX + 90, 300 + selectPanelY, 1, selectionDisabledColour, 1));
			if (uc->getGunLevel() < 5) {
				constantTextUI.push_back(Core::renderEngine->ui->addText("Chassis Upgrade Required", statPanelX + 50, 340 + statPanelY, 0.5, unavailableTextColour, 1));
			} else {
				constantTextUI.push_back(Core::renderEngine->ui->addText("Fully Upgraded", statPanelX + 50, 340 + statPanelY, 0.5, generalTextColour, 1));
			}
		}

		// Check if armor should be active
		if (uc->canUpgradeType(ARMOR_UPGRADE)) {
			upgrades.push_back(ARMOR_UPGRADE);
			currentTextUiIds.push_back(renderEngine->ui->addText("Armor", selectPanelX + 90, 300 + selectPanelY*2, 1, generalTextColour, 1));
			constantTextUI.push_back(Core::renderEngine->ui->addText("Upgrade Available", statPanelX + 50, 340 + statPanelY*2, 0.5, availableTextColour, 1));
		} else {
			constantTextUI.push_back(renderEngine->ui->addText("Armor", selectPanelX + 90, 300 + selectPanelY*2, 1, selectionDisabledColour, 1));
			if (uc->getArmorLevel() < 5) {
				constantTextUI.push_back(Core::renderEngine->ui->addText("Chassis Upgrade Required", statPanelX + 50, 340 + statPanelY*2, 0.5, unavailableTextColour, 1));
			} else {
				constantTextUI.push_back(Core::renderEngine->ui->addText("Fully Upgraded", statPanelX + 50, 340 + statPanelY*2, 0.5, generalTextColour, 1));
			}
		}

		// Check if ram should be active
		if (uc->canUpgradeType(RAM_UPGRADE)) {
			upgrades.push_back(RAM_UPGRADE);
			currentTextUiIds.push_back(renderEngine->ui->addText("Ram", selectPanelX + 90, 300 + selectPanelY*3, 1, generalTextColour, 1));
			constantTextUI.push_back(Core::renderEngine->ui->addText("Upgrade Available", statPanelX + 50, 340 + statPanelY*3, 0.5, availableTextColour, 1));
		} else {
			constantTextUI.push_back(renderEngine->ui->addText("Ram", selectPanelX + 90, 300 + selectPanelY*3, 1, selectionDisabledColour, 1));
			if (uc->getRamLevel() < 5) {
				constantTextUI.push_back(Core::renderEngine->ui->addText("Chassis Upgrade Required", statPanelX + 50, 340 + statPanelY*3, 0.5, unavailableTextColour, 1));
			} else {
				constantTextUI.push_back(Core::renderEngine->ui->addText("Fully Upgraded", statPanelX + 50, 340 + statPanelY*3, 0.5, generalTextColour, 1));
			}
		}

		maxChoiceIndex = upgrades.size();
		currentChoiceIndex = 0;

		// Render any final stat panel stuff
		std::ostringstream oss;
		oss << "Chassis lvl: " << uc->getChassisLevel();
		std::string chassisLevel = oss.str();
		constantTextUI.push_back(Core::renderEngine->ui->addText(chassisLevel, statPanelX + 50, 305, 0.65, generalTextColour, 1));
		oss.str("");
		oss.clear();

		oss << "Gun lvl: " << uc->getGunLevel();
		std::string gunLevel = oss.str();
		constantTextUI.push_back(Core::renderEngine->ui->addText(gunLevel, statPanelX + 50, 305 + statPanelY, 0.65, generalTextColour, 1));
		oss.str("");
		oss.clear();

		oss << "Armor lvl: " << uc->getArmorLevel();
		std::string armorLevel = oss.str();
		constantTextUI.push_back(Core::renderEngine->ui->addText(armorLevel, statPanelX + 50, 305 + statPanelY*2, 0.65, generalTextColour, 1));
		oss.str("");
		oss.clear();

		oss << "Ram lvl: " << uc->getRamLevel();
		std::string ramLevel = oss.str();
		constantTextUI.push_back(Core::renderEngine->ui->addText(ramLevel, statPanelX + 50, 305 + statPanelY*3, 0.65, generalTextColour, 1));
		oss.str("");
		oss.clear();

		oss << "Total lvl: " << (uc->getRamLevel() + uc->getArmorLevel() + uc->getChassisLevel() + uc->getGunLevel());
		std::string totalLevel = oss.str();
		constantTextUI.push_back(renderEngine->ui->addText(totalLevel, 150, 230, 1, generalTextColour, 1));
		oss.str("");
		oss.clear();

		upgradeMenuInitialized = true;
	}

	int present = glfwJoystickPresent(GLFW_JOYSTICK_1);
	if (present) {
		int buttonCount, axesCount;
		const float *axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axesCount);
		const unsigned char *buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);
		if ((buttons[GAMEPAD_DPAD_DOWN] || axes[GAMEPAD_LEFT_JOYSTICK_Y] < -0.5f) && uc->isUpgradeAvailable()) {
			if (!previouslyPressed) {
				currentChoiceIndex = (currentChoiceIndex + 1) % maxChoiceIndex;
			}
			previouslyPressed = true;
		}
		else if ((buttons[GAMEPAD_DPAD_UP] || axes[GAMEPAD_LEFT_JOYSTICK_Y] > 0.5f) && uc->isUpgradeAvailable()) {
			if (!previouslyPressed) {
				currentChoiceIndex = currentChoiceIndex == 0 ? maxChoiceIndex - 1 : currentChoiceIndex - 1;
			}
			previouslyPressed = true;
		}
		else if (buttons[GAMEPAD_A] && uc->isUpgradeAvailable()) {
			if (!previouslyPressed) {
				enterPressed = true;
			}
			previouslyPressed = true;
		}
        else if (buttons[GAMEPAD_X]) {
            if (!previouslyPressed) {
                pauseButtonPressed = true;
            }
            previouslyPressed = true;
        }
		else {
			previouslyPressed = false;
		}
	}
	else { // KEYBOARD IMPLEMENTATION
		if (keyPressedUp && uc->isUpgradeAvailable()) {
			currentChoiceIndex = currentChoiceIndex == 0 ? maxChoiceIndex - 1 : currentChoiceIndex - 1;
			keyPressedUp = false;
		}
		else if (keyPressedDown && uc->isUpgradeAvailable()) {
			currentChoiceIndex = (currentChoiceIndex + 1) % maxChoiceIndex;
			keyPressedDown = false;
		}
	}

	if (enterPressed && uc->isUpgradeAvailable()) {
		enterPressed = false;
		if (uc->canUpgradeType(upgrades[currentChoiceIndex]))
			uc->upgradeVehicle(upgrades[currentChoiceIndex]);
		upgradeMenuInitialized = false;

		for (int i = 0; i < currentImageUiIds.size(); ++i) {
			renderEngine->ui->removeImage(currentImageUiIds[i]);
		}
		currentImageUiIds.clear();
		for (int i = 0; i < currentTextUiIds.size(); ++i) {
			renderEngine->ui->removeText(currentTextUiIds[i]);
		}
		currentTextUiIds.clear();
		for (int i = 0; i < constantTextUI.size(); ++i) {
			renderEngine->ui->removeText(constantTextUI[i]);
		}
		constantTextUI.clear();
	} else {
	    enterPressed = false;
	}


    if (pauseButtonPressed || upgradeButtonPressed) {
        upgradeMenuInitialized = false;
        pauseButtonPressed = false;
        upgradeButtonPressed = false;
        properties.isInUpgradeMenu = false;
        properties.isPaused = false;
        upgradeMenuInitialized = false;

        for (int i = 0; i < currentImageUiIds.size(); ++i) {
            renderEngine->ui->removeImage(currentImageUiIds[i]);
        }
        currentImageUiIds.clear();
        for (int i = 0; i < currentTextUiIds.size(); ++i) {
            renderEngine->ui->removeText(currentTextUiIds[i]);
        }
        currentTextUiIds.clear();
        for (int i = 0; i < constantTextUI.size(); ++i) {
            renderEngine->ui->removeText(constantTextUI[i]);
        }
        constantTextUI.clear();
    }

	// Make the current selection green
	for (int i = 0; i < currentTextUiIds.size(); ++i) {
		if (i == currentChoiceIndex) {
			renderEngine->ui->modifyText(currentTextUiIds[i], nullptr, nullptr, nullptr, nullptr, &menuSelectedTextColor, nullptr);
		}
		else {
			renderEngine->ui->modifyText(currentTextUiIds[i], nullptr, nullptr, nullptr, nullptr, &menuBaseTextColor, nullptr);
		}
	}
}

