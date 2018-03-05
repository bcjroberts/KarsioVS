#pragma once

#include <string>
#include <map>
#include <glm/glm.hpp>
//#include <glm/detail/type_vec2.hpp>
#include "../../Main/initOpenGL/OpenGL_Deps.h"
#include <vector>

class UserInterface {
private:
	struct Text {
		std::string contents;
		GLfloat xpos;
		GLfloat ypos;
		GLfloat scale;
		glm::vec3 color;
	};
	std::vector<Text> textArray;
	
	// Holds all state information relevant to a character as loaded using FreeType
	struct Character {
		GLuint textureID;	// ID handle of the glyph texture
		glm::ivec2 size;    // Size of glyph
		glm::ivec2 bearing; // left/top of glyph offset
		GLuint advance;	// Horizontal offset to advance to next glyph
	};
	int *width;
	int *height;
	// Holds a list of pre-compiled Characters
	std::map<GLchar, Character> characters;
	GLuint fontShaderID;
	// Render state
	GLuint VAO, VBO;
	
	void renderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color = glm::vec3(1.0f));
public:
	UserInterface(int *width, int *height);

	int addText(std::string text, GLfloat x, GLfloat y, GLfloat scale = 1.0f, glm::vec3 color = glm::vec3(1.0f));
	void removeText(int index);
	void modifyText(int index, std::string* contents, int* xpos, int* ypos, int* scale, glm::vec3* color);

	void renderTextArray();

	void loadFont(std::string font, GLuint fontSize);
	~UserInterface() = default;
};
