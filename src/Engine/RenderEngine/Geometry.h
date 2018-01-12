//
// Created by Sebastian on 1/4/2018.
//

#ifndef PROJECTKARSIO_GEOMETRY_H
#define PROJECTKARSIO_GEOMETRY_H


#include "../../Main/initOpenGL/OpenGL_Deps.h"
#include "../../Art/MeshData.h"
//#include "ArtData/RenderData.h"
//#include "../../Art/Mesh.h"

class Geometry {
private:
    GLuint VAO, VBO, EBO;

public:
    MeshData *meshDataPointer; //used as an ID for whether the model exists
    int triangleCount;

    Geometry();
    void makeBuffer(MeshData &mesh);

    void bindBuffer();

    ~Geometry();

    void draw();
};


#endif //PROJECTKARSIO_GEOMETRY_H
