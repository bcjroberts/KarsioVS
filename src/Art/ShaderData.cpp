//
// Created by Sebastian on 06/01/2018.
//

#include "ShaderData.h"


ShaderData::ShaderData() = default;

ShaderData::~ShaderData(){
    id = 0;
}

bool ShaderData::attachShader(std::string path, GLenum type){
    if (id == 0) {
        id = glCreateProgram();
        //if (OpenGL::error("glCreateProgram"))
        //	return false;
    }
//	cout<<id<<endl;

    std::ifstream in(path);
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
    glAttachShader(id, shader);
//    cout<<id<<":"<<shader<<endl;
    //if (OpenGL::error("glAttachShader"))
    //	return false;
    return true;
}

bool ShaderData::link(){
    if (id <= 0) {
        return false;
    }
    glLinkProgram(id);
    //if (OpenGL::error("glLinkProgram"))
    //	return false;
//    cout<<"Linking: "<<id<<endl;

    GLint state = 0;
    glGetProgramiv(id, GL_LINK_STATUS, &state);
    if (state == GL_FALSE) {
        glGetProgramiv(id, GL_INFO_LOG_LENGTH, &state);

        char* buffer = new char[state];
        glGetProgramInfoLog(id, state, &state, buffer);
        buffer[state - 1] = '\0';         // ensure we're null-terminated

        std::cerr << "ERROR: Program " << id << " did not link. The program log:" << std::endl;
        std::cerr << buffer << std::endl;

        return false;
    }

    return true;
}

bool ShaderData::use(){
    glUseProgram(id);
    //if (OpenGL::error("glUseProgram"))
    //	return false;
    return true;
}

void ShaderData::unbind(){
    glUseProgram(0);
}