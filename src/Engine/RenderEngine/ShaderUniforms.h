/*
 * Written in a functional programming sense, 
 * only affects gpu stuff by setting a shader uniform
 * Shader ID needs to be passed into each function
 */


#pragma once
#include <string>
#include "../../Main/initOpenGL/OpenGL_Deps.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class ShaderUniforms {
public:
	ShaderUniforms();
	~ShaderUniforms();
	// utility uniform functions
	static void setBool(const GLuint shaderID, const std::string& name, bool value);
	// ------------------------------------------------------------------------
	static void setInt(const GLuint shaderID, const std::string& name, int value);
	// ------------------------------------------------------------------------
	static void setFloat(const GLuint shaderID, const std::string& name, float value);
	// ------------------------------------------------------------------------
	static void setVec2(const GLuint shaderID, const std::string& name, const glm::vec2& value);
	static void setVec2(const GLuint shaderID, const std::string& name, float x, float y);
	// ------------------------------------------------------------------------
	static void setVec3(const GLuint shaderID, const std::string& name, const glm::vec3& value);
	static void setVec3(const GLuint shaderID, const std::string& name, float x, float y, float z);
	// ------------------------------------------------------------------------
	static void setVec4(const GLuint shaderID, const std::string& name, const glm::vec4& value);
	static void setVec4(const GLuint shaderID, const std::string& name, float x, float y, float z, float w);
	// ------------------------------------------------------------------------
	static void setMat2(const GLuint shaderID, const std::string& name, const glm::mat2& mat);
	// ------------------------------------------------------------------------
	static void setMat3(const GLuint shaderID, const std::string& name, const glm::mat3& mat);
	// ------------------------------------------------------------------------
	static void setMat4(const GLuint shaderID, const std::string& name, const glm::mat4& mat);

	static void setMat4(const GLuint shaderID, const std::string & name, const glm::mat4 & mat, const bool glTrueFalse);
};
