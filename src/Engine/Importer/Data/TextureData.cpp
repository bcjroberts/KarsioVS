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

void TextureData::loadTexture(std::string filename) {
//	GLuint textureID;
	int imageLayers, imageWidth, imageHeight;
	//Bind texture
	glGenTextures(1, &textureID);

	unsigned char* image = stbi_load(filename.c_str(), &imageWidth, &imageHeight, &imageLayers, STBI_rgb);

	if (image) {

		GLenum format;
		if (imageLayers == 1) {
			format = GL_RED;
		} 
		else if (imageLayers == 3) {
			format = GL_RGB;
		} 
		else {
			format = GL_RGBA;
		}

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, imageWidth, imageHeight, 0, format, GL_UNSIGNED_BYTE, image);
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
