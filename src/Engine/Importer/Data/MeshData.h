#pragma once



#include <vector>

// Include glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Include correct assimp folder
#include <assimp/mesh.h>
// Include opengl stuff
#include <GL/glew.h>



class MeshData {
public:	
	// Vertex data structure
	struct Vertex {
		glm::vec3 position; // point location
		glm::vec3 normal;   // vertex normal
		glm::vec2 uvCoords; // vertex uv coordinate
		glm::vec3 tangent;	// tangent
		glm::vec3 bitangent; // bitangent
		glm::vec3 vertColors; // vertex colors 
	};
	MeshData(aiMesh *mesh);
	~MeshData() = default;

	// Data containers for everything that makes up a mesh
	// Maybe make some kind of nice functions that can 
	//  return simple arrays for rendering
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	unsigned int VAO;

private:
	unsigned int VBO, EBO;

	// Functions for filling the above data
	void setVerticies(aiMesh *mesh);
	void setIndicies(aiMesh *mesh);
	void setupBuffers();
};
