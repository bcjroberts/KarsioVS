//
// This will start the game component of the project ignoring the editor.
//

#include <iostream>
#include "../Engine/Core.h"
#include "initOpenGL/OpenGL.h"

int main(){
    int screenWidth = 1024;
    int screenHeight = 1024;

    OpenGL openGL_Program(screenWidth,screenHeight,"Project Karsio");

    Core core(screenWidth,screenHeight,openGL_Program.getWindow(),false);
    core.coreLoop();

    //final thing that will be printed to inform the programmer that stuff didn't crash ungracefully
    std::cout<<"exiting project"<<std::endl;
    return 0;
}