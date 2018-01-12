//
// Created by Sebastian on 1/4/2018.
//

#include <fstream>
#include "MeshData.h"



MeshData::MeshData(std::string name) {
    //model_ID = name;
    //meshDataPointer = this;
    //loadModelData(filepath);
}

void MeshData::loadMeshData(std::string filepath) {
    openOBJ(filepath);
}


//Following function based on:
// http://www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading/
// https://stackoverflow.com/questions/21120699/c-obj-file-parser
void MeshData::openOBJ(std::string filename) {
    std::vector<int> vertexIndices, uvIndices, normalIndices;
    std::vector<vec3> temp_vertices;
    std::vector<vec3> temp_normals;
    std::vector<vec2> temp_uvs;

    // try to open the file
    std::ifstream file;
    file.open( filename, std::fstream::in );
    // didn't work? fail!
    if ( file.fail() ) {
        std::cout << "ERROR: OBJmodel: Couldn't load \""
             << filename << "\"." << std::endl;
    }

    std::string line; // a buffer to store lines in, and separators
    const char* values;
    //Process the file
    while(getline(file,line)) {	// while we have data, read in a line

        if (line.substr(0,2)=="v "){
            vec3 vertex;
            values = line.c_str();
            sscanf(values, "v %f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            //cout<<"V: "<<vertex.x<<" "<<vertex.y<<" "<<vertex.z<<endl;
            temp_vertices.push_back(vertex);
            //updateBoundingBox(vertex);
        }
            //Process uv coordinates at each vertex
        else if (line.substr(0,2)=="vt"){
            vec2 uv;
            values = line.c_str();
            sscanf(values, "vt %f %f\n", &uv.x, &uv.y);
            //cout<<"UV: "<<uv.x<<" "<<uv.y<<endl;
            temp_uvs.push_back(uv);
        }
            //Process normals at each vertex
        else if (line.substr(0,2)=="vn"){
            vec3 normal;
            values = line.c_str();
            sscanf(values, "vn %f %f %f\n", &normal.x, &normal.y, &normal.z);
            //cout<<"N: "<<normal.x<<" "<<normal.y<<" "<<normal.z<<endl;
            temp_normals.push_back(normal);
        }
            //Process indices of each set of vertices, uvs, and normals.
        else if (line.substr(0,2)=="f "){
            //string vertex1, vertex2, vertex3;
            values = line.c_str();
            int vertexIndex[3], uvIndex[3], normalIndex[3];
            sscanf(values, "f %d/%d/%d %d/%d/%d %d/%d/%d\n",
                   &vertexIndex[0], &uvIndex[0], &normalIndex[0],
                   &vertexIndex[1], &uvIndex[1], &normalIndex[1],
                   &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
            vertexIndices.push_back(vertexIndex[0] - 1);
            vertexIndices.push_back(vertexIndex[1] - 1);
            vertexIndices.push_back(vertexIndex[2] - 1);
            uvIndices.push_back(uvIndex[0] - 1);
            uvIndices.push_back(uvIndex[1] - 1);
            uvIndices.push_back(uvIndex[2] - 1);
            normalIndices.push_back(normalIndex[0] - 1);
            normalIndices.push_back(normalIndex[1] - 1);
            normalIndices.push_back(normalIndex[2] - 1);
        }
    }
    Vertex vertex;

    //Store the file's contents in proper MeshData setup
    for (int i = 0; i < vertexIndices.size(); ++i) {
        vertex.position = temp_vertices[vertexIndices[i]];
        vertex.normal = temp_normals[normalIndices[i]];
        vertex.uvCoords = temp_uvs[uvIndices[i]];
        vertices.push_back(vertex);
    }
}