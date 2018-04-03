#include "TextureData.h"
#include <iostream>

//include STB for opening the texture
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

TextureData::TextureData()
{
}


TextureData::~TextureData()
{
}

bool TextureData::openTexture(unsigned char* &image, int &imageLayers, std::string &filename, GLenum &openFormat, GLenum &useFormat) {
	image = stbi_load(filename.c_str(), &imageWidth, &imageHeight, &imageLayers, STBI_rgb);
	if (image) {
		if (imageLayers == 1) {
			openFormat = GL_RED;
			useFormat = GL_RED;
		}
		else if (imageLayers == 3) {
			openFormat = GL_RGB;
			useFormat = GL_RGB;
		}
		else {
			image = stbi_load(filename.c_str(), &imageWidth, &imageHeight, &imageLayers, STBI_rgb_alpha);
			openFormat = GL_RGBA;
			useFormat = GL_RGBA;
		}
		return true;
	}
	else {
		std::cout << "Failed to load texture: " << filename << std::endl;
		return false;
	}
}

void TextureData::loadTexture(std::string filename) {
	//	GLuint textureID;
	int imageLayers;
	unsigned char* image;
	GLenum openFormat, useFormat;
	//Bind texture
	glGenTextures(1, &textureID);
	//	std::cout << filename << std::endl;
	if(openTexture(image, imageLayers, filename, openFormat, useFormat)){
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, openFormat, imageWidth, imageHeight, 0, useFormat, GL_UNSIGNED_BYTE, image);

		glGenerateMipmap(GL_TEXTURE_2D);

		//Texture parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	}
	stbi_image_free(image);
}

void TextureData::loadCubemap(std::vector<std::string> faces) {
	for (int i = 0; i < faces.size(); ++i) {
		//	GLuint textureID;
		int imageLayers;
		unsigned char* image;
		GLenum openFormat, useFormat;
		//Bind texture
		glGenTextures(1, &textureID);
		//	std::cout << filename << std::endl;
		if (openTexture(image, imageLayers, faces[i], openFormat, useFormat)) {
			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		}
		stbi_image_free(image);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void TextureData::makeTextureColor(float pixels[], GLenum channelFormat) {

	//	std::cout << pixels[0] << std::endl;

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexImage2D(GL_TEXTURE_2D, 0, channelFormat, 1, 1, 0, channelFormat, GL_FLOAT, pixels);
	glGenerateMipmap(GL_TEXTURE_2D);

	//Texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
}
