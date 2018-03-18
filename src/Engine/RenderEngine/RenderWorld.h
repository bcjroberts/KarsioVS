#pragma once
#include <vector>
#include "../../Main/initOpenGL/OpenGL_Deps.h"
//#include "ArtData/RenderData.h"
#include "../Importer/Data/Model.h"
#include "../Importer/Data/ShaderData.h"

//Include glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//using namespace glm;

class RenderWorld {
public:
	RenderWorld();
	~RenderWorld();
	//Need the model that is being used and the instance that will be added to that model
	void renderElements();
	void updateCamera(glm::mat4& view, glm::mat4& projection, glm::vec3& location);
	void setLight(int index, glm::vec3 position, glm::vec3 color);
	int getNextAvailableLightID();
	void freeLightWithID(int index);
	void addInstance(Model& model, int id, glm::mat4 transform);
	void updateInstance(Model& model, int id, glm::mat4 transform);
	void removeInstance(Model& model, int id);
	//	void render(Camera camera);

private:
	struct CameraDetails {
		glm::mat4 view;
		glm::mat4 projection;
		glm::vec3 location;
	};
	CameraDetails cameraDetails;

	const static int NUM_LIGHTS = 50;
	bool lightsInUse[NUM_LIGHTS];

	struct Textures {
		//ALl the texture data here
		GLuint albedo;
		GLuint roughness;
		GLuint metalness;
		GLuint normal;
		GLuint emission;
	};
	struct Instance {
		int ID; //Is this needed? If so maybe also use this to figure out whether a given instance is an actual object?
		glm::mat4 transform; //some kind of info regarding where to draw the object
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
		glm::vec3 position;
		glm::vec3 color;
//		double distance;
		//		vec3 falloff;
	}; 
	std::vector<Light> lights;

//	GLFWwindow *window;
//	int *screenWidth;
//	int *screenHeight;

	void passLights(GLuint shaderID);
	
	void passCamera(GLuint shaderID);

	void passTextures(GLuint shaderID);

	void activateTextures(RendererModel sModel);

};
