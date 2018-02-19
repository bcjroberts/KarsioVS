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

using namespace glm;

class RenderEngine{
private:
    struct Material{

        //ALl the texture data here
    };
//    struct Mesh{
//        Model *meshDataPointer; //used as an ID for whether the model exists
//        std::vector<int> vertexIndices;
//        std::vector<int> uvIndices;
//        std::vector<int> normalIndices;
//        std::vector<vec3> verticies;
//        std::vector<vec3> normals;
//        std::vector<vec2> uvs;
//    };

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
        Material materialData;
        //Mesh meshData;
//        Geometry geometry;
		std::vector<Geometry> meshes;
        std::vector<Instance> instances; //list of instances that will be drawn using a single model
    };
    std::vector<RendererModel> sceneModels; //list of all the models in the scene

    GLFWwindow *window;

    void renderElements(Camera camera);

public:
    RenderEngine(GLFWwindow *window);
    ~RenderEngine();

    void render(Camera camera);
    //void removeInstance(RenderData model);

    //Need the model that is being used and the instance that will be added to that model
    void addInstance(Model &model, int id, mat4 transform);
    void updateInstance(Model &model, int id, mat4 transform);
};


#endif //RENDERENGINE_RENDERENGINE_H
