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
//Used for my not-so-great struct -Brian
#include "../Game/Components/DriveComponent.h"
// for A*
#include "../Game/Logic/AStar.h"
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

    case GLFW_KEY_RIGHT:
        tempPlayerInput.steerRight = set ? 1 : 0;
        break;
    case GLFW_KEY_LEFT:
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
    PhysicsEngine physicsEngine;
    AudioEngine audioEngine;
    Logic logic;

	glfwSetKeyCallback(properties.window, windowKeyInput);
	glfwSetInputMode(properties.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    physicsEngine.initPhysics();
    // -----------Temp code, to initialize model/instance in rendering code...
    // Obviously this should be moved elsewhere when it's being used for real...

    //initialize camera class, in theory if you modify the position/direction of
    // the camera class it will affect the rendered view
    Camera camera(properties.screenWidth,properties.screenHeight);

    //following set of functions opens an object file, I considered sticking an id
    // of some kind into the mesh data (hence the constructor takes a string) but
    // in actually that's never used
    MeshData chassisMesh("testObjectChassis");
    chassisMesh.loadMeshData("data/assets/meshes/chassis.obj");
	MeshData wheelMesh("testObjectWheels");
	wheelMesh.loadMeshData("data/assets/meshes/wheels.obj");
	MeshData cubeMesh("testObject1");
	cubeMesh.loadMeshData("data/assets/meshes/cube.obj");
    MeshData planeMesh("planeMesh");
    planeMesh.loadMeshData("data/assets/meshes/plane.obj");
	
	/*
	// Should go somewhere else I am sure; logic?? Will put here for now
	AStar::Generator generator;
	//set 2D map size
	generator.setWorldSize(vec2(25, 25));
	//generator.setHeuristic(AStar::Heuristic::euclidean); // shouldn't need this, always euclidean
	generator.setDiagonalMovement(true);
	generator.addCollision(vec2(1, 1));
	generator.addCollision(vec2(18, 18));

	// returns vector of coordinates from target to source
	auto path = generator.findPath(vec2(0, 0), vec2(20, 20));
	
	for (auto& coordinate : path) {
		std::cout << coordinate.x << " " << coordinate.y << "\n";
	}
	*/
    //Following set of functions adds the shaders to the shader class and then links them
    ShaderData shaderData;
    shaderData.attachShader("data/shaderData/vertex.glsl",GL_VERTEX_SHADER);
    shaderData.attachShader("data/shaderData/fragment.glsl",GL_FRAGMENT_SHADER);
    shaderData.link();


    // Brian's shenanigans go here
    // Don't mind me, just trying to figure out how to shove everything into logic
    logic.bindCamera(&camera);
    // END Brian's shenanigans


    vehicleData* myVehicleData = physicsEngine.createVehicle(physx::PxVec3(0,0,0));
    //printf("Number of shapes: %i", myVehicle->getRigidDynamicActor()->getNbShapes());
    physx::PxShape* shapes[5];
    physx::PxRigidDynamic* rigid1 = myVehicleData->myVehicle->getRigidDynamicActor();
    rigid1->getShapes(shapes, 5);

	// New Entity creation code, place at center of screen, no rotation, scale of 1.
    auto entity1 = EntityManager::getInstance()->createEntity(glm::vec3(0.f), glm::quat(), glm::vec3(1.f));
    //ComponentManager::getInstance()->addRendererComponent(entity1, &cubeMesh, &shaderData, glm::vec3(0,0,0),glm::quat(glm::vec3(0, -1.57, 0)),glm::vec3(2.5f, 1.0f, 1.25f));
    ComponentManager::getInstance()->addShapeRendererComponent(entity1, &chassisMesh, &shaderData, shapes[4], rigid1, glm::vec3(1.0f, 1.0f, 1.0f));
	ComponentManager::getInstance()->addShapeRendererComponent(entity1, &wheelMesh, &shaderData, shapes[0], rigid1, glm::vec3(1.0f, 1.0f, 1.0f));
	ComponentManager::getInstance()->addShapeRendererComponent(entity1, &wheelMesh, &shaderData, shapes[1], rigid1, glm::vec3(1.0f, 1.0f, 1.0f));
    ComponentManager::getInstance()->addShapeRendererComponent(entity1, &wheelMesh, &shaderData, shapes[2], rigid1, glm::vec3(1.0f, 1.0f, 1.0f));
    ComponentManager::getInstance()->addShapeRendererComponent(entity1, &wheelMesh, &shaderData, shapes[3], rigid1, glm::vec3(1.0f, 1.0f, 1.0f));
    ComponentManager::getInstance()->addPhysicsComponent(entity1, rigid1);
    ComponentManager::getInstance()->addDriveComponent(entity1, &myVehicleData->myInput);

	
	// Making a second vehicle:
    vehicleData* myVehicleData2 = physicsEngine.createVehicle(physx::PxVec3(-5.0f, 0, 0));
	physx::PxShape* shapes2[5];
    physx::PxRigidDynamic* rigid2 = myVehicleData2->myVehicle->getRigidDynamicActor();
    rigid2->getShapes(shapes2, 5);
	
	// New Entity creation code, place at center of screen, no rotation, scale of 1.
	auto entity3 = EntityManager::getInstance()->createEntity(glm::vec3(0.f), glm::quat(), glm::vec3(1.f));
	//ComponentManager::getInstance()->addRendererComponent(entity1, &cubeMesh, &shaderData, glm::vec3(0,0,0),glm::quat(glm::vec3(0, -1.57, 0)),glm::vec3(2.5f, 1.0f, 1.25f));
	ComponentManager::getInstance()->addShapeRendererComponent(entity3, &cubeMesh, &shaderData, shapes2[4], rigid2, glm::vec3(1.25f, 1.0f, 2.5f));
	ComponentManager::getInstance()->addShapeRendererComponent(entity3, &cubeMesh, &shaderData, shapes2[0], rigid2, glm::vec3(0.5f, 0.5f, 0.5f));
	ComponentManager::getInstance()->addShapeRendererComponent(entity3, &cubeMesh, &shaderData, shapes2[1], rigid2, glm::vec3(0.5f, 0.5f, 0.5f));
	ComponentManager::getInstance()->addShapeRendererComponent(entity3, &cubeMesh, &shaderData, shapes2[2], rigid2, glm::vec3(0.5f, 0.5f, 0.5f));
	ComponentManager::getInstance()->addShapeRendererComponent(entity3, &cubeMesh, &shaderData, shapes2[3], rigid2, glm::vec3(0.5f, 0.5f, 0.5f));
	ComponentManager::getInstance()->addPhysicsComponent(entity3, rigid2);
	
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
                physicsEngine.simulateTimeInSeconds(physicsTimeStep);
                //const physx::PxVec3 pos = myVehicle->getRigidDynamicActor()->getGlobalPose().p;
                //printf("Vehicle position: (%f, %f, %f)\n", pos.x, pos.y, pos.z);
            }
            // Move the camera to track the vehicle
			
            const physx::PxVec3 pos = myVehicleData->myVehicle->getRigidDynamicActor()->getGlobalPose().p;
            //camera.moveCamera(movement, pos.z - previousZ);
            //previousZ = pos.z;
            
			// Move camera by keyboard and cursor
			camera.lookAtPos = glm::vec3(pos.x, pos.y, pos.z);
			glfwGetCursorPos(properties.window, &xpos, &ypos);
			camera.rotateView(vec2(xpos / properties.screenWidth, ypos / properties.screenHeight));
			//camera.moveCamera(movement, 0.5f); // just some number for the time delta...
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
