#pragma once
#include <string>
#include "../../../Main/initOpenGL/OpenGL_Deps.h"
#include <vector>

class TextureData {
private:
	//	void createTextureFromFile(unsigned char* image, GLenum open_format, int image_width, int image_height, GLenum use_format);

public:
	GLuint textureID;
	int imageWidth = 0;
	int imageHeight = 0;

	TextureData();
	~TextureData();
	void loadTexture(std::string filename);
	void makeTextureColor(float pixels[], GLenum channelFormat);
};

