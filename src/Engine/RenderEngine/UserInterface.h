#pragma once

#include <string>
#include <map>
#include <glm/glm.hpp>
//#include <glm/detail/type_vec2.hpp>
#include "../../Main/initOpenGL/OpenGL_Deps.h"
#include <vector>
#include "../Importer/Data/TextureData.h"

class UserInterface {
private:
	struct Element {
		GLfloat xPos;
		GLfloat yPos;
		GLfloat zLevel;
		GLfloat scale;

		Element(GLfloat x, GLfloat y, GLfloat z, GLfloat scale) {
			xPos = x;
			yPos = y;
			zLevel = z;
			this->scale = scale;
		}
	};

	struct Image : Element {
		GLuint imageID;
		int width;
		int height;
        int myId;

		Image(TextureData image, GLfloat x, GLfloat y, GLfloat z, GLfloat scale) : Element(x, y, z, scale) {
			this->imageID = image.textureID;
			this->width = image.imageWidth;
			this->height = image.imageHeight;
		}
	};

	std::vector<Image> imageArray;

	struct Text : Element {
		std::string contents;
		glm::vec3 color;
	    int myId;

	    Text(std::string contents, glm::vec3 color, GLfloat x, GLfloat y, GLfloat z, GLfloat scale) : Element(x, y, z, scale) {
			this->contents = contents;
			this->color = color;
		}
	};

	std::vector<Text> textArray;

	// Holds all state information relevant to a character as loaded using FreeType
	struct Character {
		GLuint textureID; // ID handle of the glyph texture
		glm::ivec2 size; // Size of glyph
		glm::ivec2 bearing; // left/top of glyph offset
		GLuint advance; // Horizontal offset to advance to next glyph
	};

	int windowWidth;
	int windowHeight;
	// Holds a list of pre-compiled Characters
	std::map<GLchar, Character> characters;

	GLuint fontShaderID;
	GLuint imageShaderID;

	// Render state
	GLuint VAO, VBO, EBO;
	//	GLuint VAO1, VBO1, EBO1;
	//	GLuint VAO2, VBO2;

	void createSquare();

	void renderImage(Image image);
	void renderText(Text text);

public:
	UserInterface(int* width, int* height);

	int addText(std::string text, GLfloat x, GLfloat y, GLfloat scale = 1.0f, glm::vec3 color = glm::vec3(1.0f), GLfloat z = 1.0f);
	void removeText(int textId);
	void modifyText(int textId, std::string* contents = nullptr, GLfloat* xpos = nullptr, GLfloat* ypos = nullptr,
		GLfloat* scale = nullptr, glm::vec3* color = nullptr, GLfloat* zLevel = nullptr);

	int addImage(TextureData image, GLfloat x, GLfloat y, GLfloat scale = 1.0f, GLfloat z = 0.5f);
	void removeImage(int imageId);
	void modifyImage(int imageId, GLfloat* xPos = nullptr, GLfloat* yPos = nullptr, GLfloat* scale = nullptr, GLfloat* zLevel = nullptr);

	void renderUI();

	void loadFont(std::string font, GLuint fontSize);
	~UserInterface() = default;
    void clearAllUI();
};
