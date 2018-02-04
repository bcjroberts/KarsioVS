//
// Created by Sebastian on 06/01/2018.
//

#include "ShaderData.h"


ShaderData::ShaderData() = default;

ShaderData::~ShaderData(){
    shaderID = 0;
}

void ShaderData::attachShader(std::string path, GLenum type){
    if (shaderID == 0) {
        shaderID = glCreateProgram();
        //if (OpenGL::error("glCreateProgram"))
        //	return false;
    }
//	cout<<shaderID<<endl;

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

//bool ShaderData::use(){
//    glUseProgram(shaderID);
//    //if (OpenGL::error("glUseProgram"))
//    //	return false;
//    return true;
//}

void ShaderData::unbind(){
    glUseProgram(0);
}