# KarsioVS
Driving game using PhysX and OpenGL

To link with visual studio follow these steps:
1. Open the project porperties panel.
2. Click on "VC++" Directories
	a. Edit "Include Directories", add the following string: 

$(PROJECTDIR)..\deps\glew-2.1.0\include;$(PROJECTDIR)..\deps\glfw3\include;$(PROJECTDIR)..\deps\glm;$(PROJECTDIR)..\deps\stb;$(PROJECTDIR)..\deps\physx\Include;$(PROJECTDIR)..\deps\physx\PxShared\include

	b. Edit "Library Directories", add the following string:

$(PROJECTDIR)..\deps\glfw3\lib-vc2015;$(PROJECTDIR)..\deps\glew-2.1.0\lib;$(PROJECTDIR)..\deps\physx\PxShared\lib\vc14win32;$(PROJECTDIR)..\deps\physx\Lib\vc14win32

3. Expand the "C/C++" tab, click on Code Generation
	
	a. Under "Runtime Library", ensure "Multi-threaded Debug DLL (/MDd)" is selected.

4. Expand the Linker tab, and click on input. Under Additional Dependencies, add the following libs:

glew32.lib
opengl32.lib
glfw3.lib
LowLevelAABBDEBUG.lib
LowLevelAABB.lib
LowLevelClothDEBUG.lib
LowLevelCloth.lib
LowLevelDEBUG.lib
LowLevelDynamicsDEBUG.lib
LowLevelDynamics.lib
LowLevel.lib
LowLevelParticlesDEBUG.lib
LowLevelParticles.lib
PhysX3CharacterKinematicDEBUG_x86.lib
PhysX3CommonDEBUG_x86.lib
PhysX3Common_x86.lib
PhysX3CookingDEBUG_x86.lib
PhysX3DEBUG_x86.lib
PhysX3ExtensionsDEBUG.lib
PhysX3VehicleDEBUG.lib
PhysX3_x86.lib
SceneQueryDEBUG.lib
SceneQuery.lib
SimulationControllerDEBUG.lib
SimulationController.lib
PxFoundationDEBUG_x86.lib
PxPvdSDKDEBUG_x86.lib
PxTaskDEBUG_x86.lib


Now make sure you click apply in the bottom right corner before clicking OK to save your settings.
Now it should build and link properly.

5. Copy all of the .dll files in the deps/dll folder into the folder with the .exe in it (Named DEBUG if you built a Debug build). This folder is only present after building. (Hotkey F5)

6. Move the data folder into the same folder as the .exe (same place as the .dlls from the previous step).
