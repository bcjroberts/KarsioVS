#pragma once
#include <string>
#include "../../../Main/initOpenGL/OpenGL_Deps.h"
#include <vector>

class TextureData {
public:
	GLuint textureID;
	TextureData();
	~TextureData();
	void loadTexture(std::string filename);
	void makeTextureColor(float pixels[], GLenum channelFormat);
};

