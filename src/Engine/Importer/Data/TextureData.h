#pragma once
#include <string>
#include "../../../Main/initOpenGL/OpenGL_Deps.h"
#include <vector>

class TextureData {
private:


public:
	GLuint textureID;
	TextureData();
	~TextureData();
	void createTextureFromFile(unsigned char* image, GLenum open_format, int image_width, int image_height, GLenum use_format);
	void loadTexture(std::string filename);
	void makeTextureColor(float pixels[], GLenum channelFormat);
};

