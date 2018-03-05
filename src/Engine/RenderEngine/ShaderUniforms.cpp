#include "ShaderUniforms.h"

ShaderUniforms::ShaderUniforms() = default;

ShaderUniforms::~ShaderUniforms() = default;

void ShaderUniforms::setBool(const GLuint shaderID, const std::string& name, bool value) {
	glUniform1i(glGetUniformLocation(shaderID, name.c_str()), (int)value);
}

// ------------------------------------------------------------------------
void ShaderUniforms::setInt(const GLuint shaderID, const std::string& name, int value) {
	glUniform1i(glGetUniformLocation(shaderID, name.c_str()), value);
}

// ------------------------------------------------------------------------
void ShaderUniforms::setFloat(const GLuint shaderID, const std::string& name, float value) {
	glUniform1f(glGetUniformLocation(shaderID, name.c_str()), value);
}

// ------------------------------------------------------------------------
void ShaderUniforms::setVec2(const GLuint shaderID, const std::string& name, const glm::vec2& value) {
	glUniform2fv(glGetUniformLocation(shaderID, name.c_str()), 1, &value[0]);
}

void ShaderUniforms::setVec2(const GLuint shaderID, const std::string& name, float x, float y) {
	glUniform2f(glGetUniformLocation(shaderID, name.c_str()), x, y);
}

// ------------------------------------------------------------------------
void ShaderUniforms::setVec3(const GLuint shaderID, const std::string& name, const glm::vec3& value) {
	glUniform3fv(glGetUniformLocation(shaderID, name.c_str()), 1, &value[0]);
}

void ShaderUniforms::setVec3(const GLuint shaderID, const std::string& name, float x, float y, float z) {
	glUniform3f(glGetUniformLocation(shaderID, name.c_str()), x, y, z);
}

// ------------------------------------------------------------------------
void ShaderUniforms::setVec4(const GLuint shaderID, const std::string& name, const glm::vec4& value) {
	glUniform4fv(glGetUniformLocation(shaderID, name.c_str()), 1, &value[0]);
}

void ShaderUniforms::setVec4(const GLuint shaderID, const std::string& name, float x, float y, float z, float w) {
	glUniform4f(glGetUniformLocation(shaderID, name.c_str()), x, y, z, w);
}

// ------------------------------------------------------------------------
void ShaderUniforms::setMat2(const GLuint shaderID, const std::string& name, const glm::mat2& mat) {
	glUniformMatrix2fv(glGetUniformLocation(shaderID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

// ------------------------------------------------------------------------
void ShaderUniforms::setMat3(const GLuint shaderID, const std::string& name, const glm::mat3& mat) {
	glUniformMatrix3fv(glGetUniformLocation(shaderID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

// ------------------------------------------------------------------------
void ShaderUniforms::setMat4(const GLuint shaderID, const std::string& name, const glm::mat4& mat) {
	glUniformMatrix4fv(glGetUniformLocation(shaderID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void ShaderUniforms::setMat4(const GLuint shaderID, const std::string& name, const glm::mat4& mat, const bool glTrueFalse) {
	glUniformMatrix4fv(glGetUniformLocation(shaderID, name.c_str()), 1, glTrueFalse, &mat[0][0]);
}
