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

using namespace glm;

class RenderEngine{
private:
	const static int NUM_LIGHTS = 50;
    bool lightsInUse[NUM_LIGHTS];

    struct Textures{
		//ALl the texture data here
		GLuint albedo;
		GLuint roughness;
		GLuint metalness;
		GLuint normal;
		GLuint emission;
    };
    struct Instance{
        int ID; //Is this needed? If so maybe also use this to figure out whether a given instance is an actual object?
        mat4 transform; //some kind of info regarding where to draw the object
    };
	struct Geometry {
		unsigned int VAO;
		unsigned int trisCount;
	};

    struct RendererModel {
        //RenderData model;
//        ShaderData shaderData;
		GLuint shaderID;
		Textures texture;
        //Mesh meshData;
//        Geometry geometry;
		std::vector<Geometry> meshes;
        std::vector<Instance> instances; //list of instances that will be drawn using a single model
    };
    std::vector<RendererModel> sceneModels; //list of all the models in the scene
	
	struct Light {
		vec3 position;
		vec3 color;
//		vec3 falloff;
	};
	
	std::vector<Light> lights;

    GLFWwindow *window;
	int *screenWidth;
	int *screenHeight;
	void passLights(GLuint shaderID);
	void passTextures(RendererModel sModel);
	void renderElements(Camera camera);

//	const void setShaderVec3(GLuint shaderID, const std::string &name, const glm::vec3 &value);
//	const void setShaderInt(GLuint shaderID, const std::string& name, int value);

public:
	//UI is public so it can be used from outside the render engine
	UserInterface * ui = nullptr;
//	void loadFont(std::string path, float size);
//	int addText(std::string contents, int xpos, int ypos, float scale = 1, glm::vec3 color = glm::vec3(1.0f));
//	void modifyText(int index, std::string* contents, int* xpos, int* ypos, float* scale, glm::vec3* color);
//	void removeText(int index);
    RenderEngine(GLFWwindow *window, int *screenWidth, int *screenHeight);
    ~RenderEngine();

    void render(Camera camera);
    //void removeInstance(RenderData model);

	void setLight(int index, vec3 position, vec3 color);
    int getNextAvailableLightID();
    void freeLightWithID(int index);
    //Need the model that is being used and the instance that will be added to that model
    void addInstance(Model &model, int id, mat4 transform);
    void updateInstance(Model &model, int id, mat4 transform);
    void removeInstance(Model& model, int id);
};


#endif //RENDERENGINE_RENDERENGINE_H
