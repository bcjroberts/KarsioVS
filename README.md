# KarsioVS
Driving game using PhysX and OpenGL

To link with visual studio follow these steps:
1. Open the project porperties panel.
2. Click on VC++ Directories
	a. Edit Include Directories, add the following string: 

$(PROJECTDIR)..\deps\glew-2.1.0\include;$(PROJECTDIR)..\deps\glfw3\include;$(PROJECTDIR)..\deps\glm;$(PROJECTDIR)..\deps\stb

	b. Edit Library Directories, add the following string:

$(PROJECTDIR)..\deps\glfw3\lib-vc2015;$(PROJECTDIR)..\deps\glew-2.1.0\lib

3. Expand the Linker tab, and click on input. Under Additional Dependencies, add the following string:

glew32.lib;opengl32.lib;glfw3.lib


Now make sure you click apply in the bottom right corner before clicking OK to save your settings.
Now it should build and link properly.

4. Copy all of the .dll files in the deps/dll folder into the folder with the .exe in it (Named DEBUG if you built a Debug build).

5. Move the data folder into the same folder as the .exe (same place as the .dlls from the previous step).
