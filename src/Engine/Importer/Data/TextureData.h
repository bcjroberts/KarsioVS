#pragma once
#include <string>
#include "../../../Main/initOpenGL/OpenGL_Deps.h"
#include <vector>

class TextureData {
private:
	//	void createTextureFromFile(unsigned char* image, GLenum open_format, int image_width, int image_height, GLenum use_format);
	bool openTexture(unsigned char*& image, int& imageLayers, std::string& filename, GLenum& openFormat, GLenum& useFormat);

public:
	GLuint textureID;
	int imageWidth = 0;
	int imageHeight = 0;
//	unsigned char pixelData;

	TextureData();
	~TextureData();
	void loadTexture(std::string filename);
	void loadCubemap(std::vector<std::string> faces);
	void makeTextureColor(float pixels[], GLenum channelFormat);
};

