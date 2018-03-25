//
// Created by Sebastian on 12/23/2017.
//

#ifndef RENDERENGINE_RENDERENGINE_H
#define RENDERENGINE_RENDERENGINE_H


#include <vector>
#include "../../Main/initOpenGL/OpenGL_Deps.h"
//#include "ArtData/RenderData.h"
#include "../Importer/Data/Model.h"
#include "../Importer/Data/ShaderData.h"
#include "../../Game/Camera.h"
//#include "Geometry.h"

//Include glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "UserInterface.h"
#include "RenderWorld.h"

//using namespace glm;

class RenderEngine{
private:
    GLFWwindow *window;
	int *screenWidth;
	int *screenHeight;

public:
	//UI is public so it can be used from outside the render engine
	UserInterface *ui = nullptr;
	RenderWorld *world = nullptr;

    RenderEngine(GLFWwindow *window, int *screenWidth, int *screenHeight);
    ~RenderEngine();

    void render(std::vector<Camera*> camera);
};


#endif //RENDERENGINE_RENDERENGINE_H
