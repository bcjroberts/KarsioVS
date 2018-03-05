//This will init and draw text to the screen

#include "UserInterface.h"

#include <glm/gtc/matrix_transform.inl>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "../Importer/Managers/ShaderDataManager.h"
#include "ShaderUniforms.h"


UserInterface::UserInterface(int *screenWidth, int *screenHeight) {
	width = screenWidth;
	height = screenHeight;

//	std::cout << width << ":" << height << std::endl;
	fontShaderID = ShaderDataManager::getShaderData("font")->shaderID;

	// Configure VAO/VBO for texture quads
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);
	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

/**
 * \brief 
 * \param text The string you want to display
 * \param x the start x position
 * \param y the start y position
 * \param scale the scale from the inital size
 * \param color the color of the text
 * \return 
 */
int UserInterface::addText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color) {
	int index = textArray.size();
	Text tempText{ text,x,y,scale,color };
	textArray.push_back(tempText);
	return index;
}

void UserInterface::removeText(int index) {
	textArray.erase(textArray.begin() + index);
}

void UserInterface::modifyText(int index, std::string* contents, int* xpos, int* ypos, int* scale, glm::vec3* color) {
	//Check if not null and then replace that value
	if(contents!=nullptr) {
		textArray[index].contents = *contents;
	}
	if (xpos != nullptr) {
		textArray[index].xpos = *xpos;
	}
	if (ypos != nullptr) {
		textArray[index].ypos = *ypos;
	}
	if (scale != nullptr) {
		textArray[index].scale = *scale;
	}
	if (color != nullptr) {
		textArray[index].color = *color;
	}
}

void UserInterface::renderTextArray() {
	for (Text text : textArray) {
		renderText(text.contents, text.xpos, text.ypos, text.scale, text.color);
	}
}

void UserInterface::renderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color) {
	// Activate corresponding render state	
	glUseProgram(fontShaderID);
	ShaderUniforms::setMat4(fontShaderID, "projection", glm::ortho(0.0f, static_cast<GLfloat>(*width),static_cast<GLfloat>(*height), 0.0f));
	ShaderUniforms::setInt(fontShaderID, "text", 0);
//	color = glm::vec3(1.0f, 1.0f, 0);
	ShaderUniforms::setVec3(fontShaderID, "textColor", color);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(this->VAO);

//	std::cout << "Font shader ID: " << fontShaderID << std::endl;
	// Iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = characters[*c];

		GLfloat xpos = x + ch.bearing.x * scale;
		GLfloat ypos = y + (this->characters['H'].bearing.y - ch.bearing.y) * scale;

		GLfloat w = ch.size.x * scale;
		GLfloat h = ch.size.y * scale;
		// Update VBO for each character
		GLfloat vertices[6][4] = {
				{ xpos,     ypos + h,   0.0, 1.0 },
				{ xpos + w, ypos,       1.0, 0.0 },
				{ xpos,     ypos,       0.0, 0.0 },

				{ xpos,     ypos + h,   0.0, 1.0 },
				{ xpos + w, ypos + h,   1.0, 1.0 },
				{ xpos + w, ypos,       1.0, 0.0 } };
		// Render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.textureID);
		// Update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 

		// Render quad
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// Now advance cursors for next glyph
		// Bitshift by 6 to get value in pixels (1/64th times 2^6 = 64)
		x += (ch.advance >> 6) * scale; 
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
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