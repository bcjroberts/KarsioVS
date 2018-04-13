

#include "MeshData.h"
#include <iostream>


MeshData::MeshData(aiMesh *mesh, bool isCrystal) {
	setVerticies(mesh, isCrystal);
	setIndicies(mesh);
	setupBuffers();
}


void MeshData::setVerticies(aiMesh* mesh, bool isCrystal) {
	// Walk through each of the mesh's vertices
	for (int i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;
		glm::vec3 tvec3; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
		
		// positions
		tvec3.x = mesh->mVertices[i].x;
		tvec3.y = mesh->mVertices[i].y;
		tvec3.z = mesh->mVertices[i].z;
		vertex.position = tvec3;

		// normals
		tvec3.x = mesh->mNormals[i].x;
		tvec3.y = mesh->mNormals[i].y;
		tvec3.z = mesh->mNormals[i].z;
		vertex.normal = tvec3;

		// texture coordinates
		// does the mesh contain texture coordinates?
		if (mesh->mTextureCoords[0]) {
			glm::vec2 tvec2;
			// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
			// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			tvec2.x = mesh->mTextureCoords[0][i].x;
			tvec2.y = mesh->mTextureCoords[0][i].y;
			vertex.uvCoords = tvec2;
		}
		else {
			vertex.uvCoords = glm::vec2(0.0f, 0.0f);
		}

		// tangent
		tvec3.x = mesh->mTangents[i].x;
		tvec3.y = mesh->mTangents[i].y;
		tvec3.z = mesh->mTangents[i].z;
		vertex.tangent = tvec3;

		// bitangent
		tvec3.x = mesh->mBitangents[i].x;
		tvec3.y = mesh->mBitangents[i].y;
		tvec3.z = mesh->mBitangents[i].z;
		vertex.bitangent = tvec3;
		
//		if(mesh->HasVertexColors(0)) {
//			//std::cout << "has vertex color" << std::endl;
//			tvec3.x = mesh->mColors[i][0].r;
//			tvec3.y = mesh->mColors[i][0].g;
//			tvec3.z = mesh->mColors[i][0].b;
//		}
//		else {
//			tvec3 = glm::vec3(1);
//		}
//		vertex.vertColors = tvec3;

		GLfloat tval = 0;
		if(isCrystal) {
			tval = 1;
		}
		vertex.isCrystal = tval;
		
		this->vertices.push_back(vertex);
	}
}

void MeshData::setIndicies(aiMesh* mesh) {
	// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		// retrieve all indices of the face and store them in the indices vector
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			this->indices.push_back(face.mIndices[j]);
	}
}

void MeshData::setupBuffers() {
	// initializes all the buffer objects/arrays

	// create buffers/arrays
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	// load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// A great thing about structs is that their memory layout is sequential for all its items.
	// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
	// again translates to 3/2 floats which translates to a byte array.
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// set the vertex attribute pointers
	// vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uvCoords));
	// vertex tangent
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
	// vertex bitangent
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));
//	// vertex colors AO
//	glEnableVertexAttribArray(5);
//	glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, vertColors));
	// vertex colors isCrystal
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, isCrystal));

	glBindVertexArray(0);
}
