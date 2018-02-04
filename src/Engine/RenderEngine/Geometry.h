//
// This will create the buffers given mesh data
//

#ifndef PROJECTKARSIO_GEOMETRY_H
#define PROJECTKARSIO_GEOMETRY_H


#include "../../Main/initOpenGL/OpenGL_Deps.h"
#include "../Importer/Data/ModelData.h"
//#include "ArtData/RenderData.h"
//#include "../../Art/Mesh.h"

class Geometry {
private:
    GLuint VAO, VBO, EBO;

public:
    ModelData *meshDataPointer; //used as an ID for whether the model exists
    int triangleCount;

    Geometry();
    void makeBuffer(ModelData &mesh);

    void bindBuffer();

    ~Geometry();

    void draw();
};


#endif //PROJECTKARSIO_GEOMETRY_H
