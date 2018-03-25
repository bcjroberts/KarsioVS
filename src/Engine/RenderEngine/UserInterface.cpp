//This will init and draw text to the screen

#include "UserInterface.h"

#include <glm/gtc/matrix_transform.inl>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "../Importer/Managers/ShaderDataManager.h"
#include "ShaderUniforms.h"


UserInterface::UserInterface(int *screenWidth, int *screenHeight) {
	windowWidth = *screenWidth;
	windowHeight = *screenHeight;
	//TODO: maybe add a feature to keep the hud the same aspect ratio as the window

	//	std::cout << width << ":" << height << std::endl;
	fontShaderID = ShaderDataManager::getShaderData("font")->shaderID;
	imageShaderID = ShaderDataManager::getShaderData("image")->shaderID;

	createSquare();
}

// Configure VAO/VBO for texture quads
void UserInterface::createSquare() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
//	float vertices[] = {
//		1.0f,  1.0f, 1.0f, 1.0f,  // top right
//		1.0f, -1.0f, 1.0f, 0.0f,  // bottom right
//		-1.0f, -1.0f, 0.0f, 0.0f,  // bottom left
//		-1.0f,  1.0f, 0.0f, 1.0f   // top left 
//	};
	float vertices[] = {
		1.0f,  0.0f, 1.0f, 1.0f,  // top right
		1.0f,  1.0f, 1.0f, 0.0f,  // bottom right
		0.0f,  1.0f, 0.0f, 0.0f,  // bottom left
		0.0f, 0.0f, 0.0f, 1.0f   // top left 
	};	
//	float vertices[] = {
//		0.5f,  0.5f, 1.0f, 1.0f,  // top right
//		0.5f, -0.5f, 1.0f, 0.0f,  // bottom right
//		-0.5f, -0.5f, 0.0f, 0.0f,  // bottom left
//		-0.5f,  0.5f, 0.0f, 1.0f   // top left 
//	};
	unsigned int indices[] = {  // note that we start from 0!
		0, 3, 1,  // first Triangle
		1, 3, 2   // second Triangle
	};

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

/**
* \brief
* \param text The string you want to display
* \param x the start x position
* \param y the start y position
* \param z the z position at which to place the object.
* \param scale the scale from the inital size
* \param color the color of the text
* \return index to text element
*/
int UserInterface::addText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color, GLfloat z) {
	int index = textArray.size();
	Text tempText(text, color, x, y, z, scale);
	textArray.push_back(tempText);
	return index;
}

void UserInterface::removeText(int index) {
	textArray.erase(textArray.begin() + index);
}

void UserInterface::modifyText(int index, std::string* contents, GLfloat* xpos, GLfloat* ypos, GLfloat* scale, glm::vec3* color, GLfloat* zLevel) {
	//Check if not null and then replace that value
	if (contents != nullptr) {
		textArray[index].contents = *contents;
	}
	if (xpos != nullptr) {
		textArray[index].xPos = *xpos;
	}
	if (ypos != nullptr) {
		textArray[index].yPos = *ypos;
	}
	if (zLevel != nullptr) {
		textArray[index].zLevel = *zLevel;
	}
	if (scale != nullptr) {
		textArray[index].scale = *scale;
	}
	if (color != nullptr) {
		textArray[index].color = *color;
	}
}

void UserInterface::renderUI() {
	for (Image image : imageArray) {
		renderImage(image);
	}

	for (Text text : textArray) {
		renderText(text);
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}



void UserInterface::renderImage(Image image) {
	glUseProgram(imageShaderID);
	ShaderUniforms::setMat4(imageShaderID, "projection", glm::ortho(0.0f, static_cast<GLfloat>(windowWidth), static_cast<GLfloat>(windowHeight), 0.0f));

	glm::mat4 transform;
	transform = glm::translate(transform, glm::vec3(image.xPos, image.yPos, 0.5f));
	transform = glm::scale(transform, glm::vec3(image.scale * image.width, image.scale * image.height, 1.0f));

	ShaderUniforms::setMat4(imageShaderID, "transformation", transform);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, image.imageID);
	ShaderUniforms::setInt(imageShaderID, "image", 0);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


}

void UserInterface::renderText(Text text) {

	glUseProgram(fontShaderID);
	ShaderUniforms::setMat4(fontShaderID, "projection", glm::ortho(0.0f, static_cast<GLfloat>(windowWidth), static_cast<GLfloat>(windowHeight), 0.0f));
	ShaderUniforms::setInt(fontShaderID, "glyph", 0);
	ShaderUniforms::setVec3(fontShaderID, "textColor", text.color);

	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);
	for (auto letter : text.contents) {

		Character character = characters[static_cast<int>(letter)];

		GLfloat xpos = text.xPos + character.bearing.x * text.scale;
		GLfloat ypos = text.yPos + (this->characters['H'].bearing.y - character.bearing.y) * text.scale;

		GLfloat w = ((character.size.x) * text.scale);
		GLfloat h = ((character.size.y) * text.scale);


		glm::mat4 transform;
		transform = glm::translate(transform, glm::vec3(xpos, ypos, 1.0f));
		transform = glm::scale(transform, glm::vec3(w, h, 1.0f));


		ShaderUniforms::setMat4(fontShaderID, "transformation", transform);
		glBindTexture(GL_TEXTURE_2D, character.textureID);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		text.xPos += (character.advance >> 6) * text.scale;
	}
}


void UserInterface::loadFont(std::string font, GLuint fontSize) {
	// First clear the previously loaded Characters
	this->characters.clear();
	// Then initialize and load the FreeType library
	FT_Library ft;
	if (FT_Init_FreeType(&ft)) // All functions return a value different than 0 whenever an error occurred
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
	// Load font as face
	FT_Face face;
	if (FT_New_Face(ft, font.c_str(), 0, &face))
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
	// Set size to load glyphs as
	FT_Set_Pixel_Sizes(face, 0, fontSize);
	// Disable byte-alignment restriction
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	// Then for the first 128 ASCII characters, pre-load/compile their characters and store them
	for (GLubyte c = 0; c < 128; c++) // lol see what I did there 
	{
		// Load character glyph 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		// Generate texture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Now store character for later use
		Character character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		characters.insert(std::pair<GLchar, Character>(c, character));
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	// Destroy FreeType once we're finished
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}

int UserInterface::addImage(TextureData image, GLfloat x, GLfloat y, GLfloat scale, GLfloat z) {
	int index = imageArray.size();
	Image tempText(image, x, y, z, scale);
	imageArray.push_back(tempText);
	return index;
}

void UserInterface::removeImage(int index) {
	imageArray.erase(imageArray.begin() + index);
}

void UserInterface::modifyImage(int index, GLfloat* xPos, GLfloat* yPos, GLfloat* scale, GLfloat* zLevel) {
	//Check if not null and then replace that value
	if (xPos != nullptr) {
		imageArray[index].xPos = *xPos;
	}
	if (yPos != nullptr) {
		imageArray[index].yPos = *yPos;
	}
	if (scale != nullptr) {
		imageArray[index].scale = *scale;
	}
	if (zLevel != nullptr) {
		imageArray[index].zLevel = *zLevel;
	}
}
