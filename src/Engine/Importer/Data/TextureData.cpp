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

//void TextureData::createTextureFromFile(unsigned char* image, GLenum open_format, int image_width, int image_height, GLenum use_format) {
//}


void TextureData::loadTexture(std::string filename) {
	//	GLuint textureID;
	int imageLayers;
	//Bind texture
	glGenTextures(1, &textureID);
	//	std::cout << filename << std::endl;
	unsigned char* image = stbi_load(filename.c_str(), &imageWidth, &imageHeight, &imageLayers, STBI_rgb);

	if (image) {

		GLenum openFormat;
		GLenum useFormat;

		if (imageLayers == 1) {
			openFormat = GL_RED;
			useFormat = GL_RED;
		}
		else if (imageLayers == 3) {
			openFormat = GL_RGB;
			useFormat = GL_RGB;
		}
		else {
			openFormat = GL_RGBA;
			useFormat = GL_RGB;
		}
		//		std::cout << openFormat << std::endl;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, openFormat, imageWidth, imageHeight, 0, useFormat, GL_UNSIGNED_BYTE, image);

		//		createTextureFromFile(image, openFormat, imageWidth,imageHeight,useFormat);
		//		glTexImage2D(GL_TEXTURE_2D, 0, format, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		//		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);

		//Texture parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);

	}
	else {
		std::cout << "Failed to load texture: " << filename << std::endl;
		exit(-1);
	}
	stbi_image_free(image);
	//	return textureID;
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
