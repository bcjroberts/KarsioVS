//
// Created by Sebastian on 1/4/2018.
//

#ifndef PROJECTKARSIO_MODELDATA_H
#define PROJECTKARSIO_MODELDATA_H

#include "../../../Main/initOpenGL/OpenGL_Deps.h"
//#include "../ShaderProgram.h"
//#include "../Geometry.h"

//Maybe move all these structs elsewhere?
//struct Mesh{
//    //vertex data
//    //indicies
//    //normals
//    //uv coords
//};

//struct Geometry{
//
//};
//struct Shaders{
//    std::string vertex;
//    std::string geometry;
//    std::string fragment;
//};


// Idea is to use PBR for most materials while still allowing for other cool effects.
// For PBR theory go here: https://www.allegorithmic.com/pbr-guide
// More PBR stuff that I'm largely basing this implementation on:
// https://seblagarde.wordpress.com/2014/04/14/dontnod-physically-based-rendering-chart-for-unreal-engine-4/
//struct TextureData{
//    GLuint albedo; //base color of material (non-shiny value range: 50-243, shiny value range: 186-255)
//    GLuint roughness; //greyscale map (0 = 100% glossy, 1 = 100% rough)
//    GLuint metallic; //greyscale map (0 = 100% nonmetal, 1 = 100% metal)
//    GLuint normal; //change the reflections by changing the normals of the faces
//    GLuint displacement; //greyscale values determining
//    GLuint emision; //what should be emitted from the surface
//};

//struct RenderData{
//    const GLuint id;
//
//    ShaderProgram shader;
//    //Geometry geometry;
//    TextureData textureData;
//
//};
//

#endif //PROJECTKARSIO_MODELDATA_H
