//
// Created by Sebastian on 06/01/2018.
//
#include <fstream>
#include <sstream>
#include "ShaderData.h"

ShaderData::~ShaderData(){
    shaderID = 0;
}

void ShaderData::attachShader(std::string shaderPath, GLenum type){
	shaderPath = "data/assets/shaderData/" + shaderPath;
//	std::cout << shaderPath << std::endl;
    if (shaderID == 0) {
        shaderID = glCreateProgram();
        //if (OpenGL::error("glCreateProgram"))
        //	return false;
    }
//	cout<<shaderID<<endl;

    std::ifstream in(shaderPath);
    std::string buffer = [&in] {
        std::ostringstream ss{};
        ss << in.rdbuf();
        return ss.str();
    }();
    const char *buffer_array[] = { buffer.c_str() };

    GLuint shader = glCreateShader(type);
    //if (OpenGL::error("glCreateShader"))
    //	return false;

    glShaderSource(shader, 1, buffer_array, 0);
    //if (OpenGL::error("glShaderSource"))
    //	return false;

    glCompileShader(shader);
    //if (OpenGL::error("glCompileShader"))
    //	return false;

    // Compile results
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        GLint length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        std::string info(length, ' ');
        glGetShaderInfoLog(shader, info.length(), &length, &info[0]);
        std::cerr << "ERROR compiling shader " << shader << " :" << std::endl;
        std::cerr << info << std::endl;
    }
    // Attach shader
    glAttachShader(shaderID, shader);
//    cout<<shaderID<<":"<<shader<<endl;
    //if (OpenGL::error("glAttachShader"))
    //	return false;
//    return true;
}

void ShaderData::link(){
    if (shaderID <= 0) {
		std::cout << "Error linking shaders" << std::endl;
//        return false;
    }
    glLinkProgram(shaderID);
    //if (OpenGL::error("glLinkProgram"))
    //	return false;
//    cout<<"Linking: "<<shaderID<<endl;

    GLint state = 0;
    glGetProgramiv(shaderID, GL_LINK_STATUS, &state);
    if (state == GL_FALSE) {
        glGetProgramiv(shaderID, GL_INFO_LOG_LENGTH, &state);

        char* buffer = new char[state];
        glGetProgramInfoLog(shaderID, state, &state, buffer);
        buffer[state - 1] = '\0';         // ensure we're null-terminated

        std::cerr << "ERROR: Program " << shaderID << " did not link. The program log:" << std::endl;
        std::cerr << buffer << std::endl;
    }
}

void ShaderData::readFile(std::ifstream &file) {
	std::string line; // a buffer to store lines in, and separators
	std::string comment = ";";
	std::string vert = "vertex=";
	std::string geom = "geometry=";
	std::string frag = "fragment=";

	while (getline(file, line)) {	// while we have data, read in a line
		if ((line.size() >= comment.size()) && (line.substr(0, comment.size()) == comment))
		{
			//if comment don't bother checking anything else and just go to the next line
		}
		else if ((line.size() >= vert.size()) && (line.substr(0, vert.size()) == vert))
		{
			// attach the shader
			std::string shaderName = line.substr(vert.size(), line.size()) + ".vert";
			attachShader(shaderName,GL_VERTEX_SHADER);
		}
		else if ((line.size() >= geom.size()) && (line.substr(0, geom.size()) == geom))
		{
			// attach the shader
			std::string shaderName = line.substr(geom.size(), line.size()) + ".geom";
			attachShader(shaderName, GL_GEOMETRY_SHADER);
		}
		else if ((line.size() >= frag.size()) && (line.substr(0, frag.size()) == frag))
		{
			// attach the shader
			std::string shaderName = line.substr(frag.size(), line.size()) + ".frag";
			attachShader(shaderName, GL_FRAGMENT_SHADER);
		}
	}
	link(); //link the shaders so that they are usuable.
}

void ShaderData::loadShader(std::string filepath) {
	//	std::string filename = filepath;
	// try to open the file
	std::ifstream file;
	file.open(filepath, std::fstream::in);
	// didn't work? fail!
	if (file.fail()) {
		std::cout << "ERROR: ModelConfig: Couldn't load: "
			<< filepath << std::endl;
	}
	readFile(file);
}

