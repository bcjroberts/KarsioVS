//
// Created by Sebastian on 12/23/2017.
//

#ifndef RENDERENGINE_CORE_H
#define RENDERENGINE_CORE_H

#include "RenderEngine/RenderEngine.h"
#include "PhysicsEngine/PhysicsEngine.h"
#include "AudioEngine/AudioEngine.h"


#include "../Main/initOpenGL/OpenGL_Deps.h"

class Core {
private:
    struct Properties{
        GLFWwindow *window; // Create glfw window
        int screenWidth,screenHeight;
        //OpenGL openGL_Program;
        bool isRunning;
        //all the stuff we might want to allow the game logic to modify but will take effect within the core engine
        bool isPaused;
    };
    Properties properties;

public:
    Core(int screenWidth,int screenHeight, GLFWwindow *window, bool gamePaused);
    void coreLoop();
    ~Core();

};


#endif //RENDERENGINE_CORE_H
