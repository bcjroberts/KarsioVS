#include "TextureDataManager.h"
#include <iostream>

std::map<std::string, TextureData*> TextureDataManager::textureDataMap;

TextureData* TextureDataManager::getTextureData(std::string textureName) {
	TextureData* result = textureDataMap[textureName];
	if (!result) {
		result = new TextureData();
//		std::cout << textureName << std::endl;
		result->loadTexture("data/assets/textures/" + textureName);
//		std::cout << result << std::endl;
		textureDataMap.insert_or_assign(textureName, result);
	}
	//	std::cout << shaderName << std::endl;
	return result;
}

GLuint TextureDataManager::texAlbedo;
GLuint TextureDataManager::texRoughness;
GLuint TextureDataManager::texMetalness;
GLuint TextureDataManager::texNormal;
GLuint TextureDataManager::texEmission;

GLuint TextureDataManager::createTexture(float pixels[], GLenum channelFormat) {
	TextureData tempTexture;
	tempTexture.makeTextureColor(pixels, channelFormat);
	return tempTexture.textureID;
}

GLuint TextureDataManager::defaultAlbedo() {
	if (texAlbedo == 0) {
		float pixels[] = { 0.75f, 0.75f, 0.75f };
		texAlbedo = createTexture(pixels, GL_RGB);
	}
	return texAlbedo;
//	std::cout << texAlbedo << std::endl;
}

GLuint TextureDataManager::defaultRoughness() {
	if (texRoughness == 0) {
		float pixels[] = { 0.5f };
		texRoughness = createTexture(pixels, GL_RED);
	}
	return texRoughness;
	//	std::cout << texAlbedo << std::endl;
}


GLuint TextureDataManager::defaultMetalness() {
	if (texMetalness == 0) {
		float pixels[] = { 0.0f };
		texMetalness = createTexture(pixels, GL_RED);
	}
	return texMetalness;
	//	std::cout << texAlbedo << std::endl;
}

GLuint TextureDataManager::defaultNormal() {
	if (texNormal == 0) {
		float pixels[] = { 0.5f, 0.5f, 1.0f };
		texNormal = createTexture(pixels, GL_RGB);
	}
	return texNormal;
	//	std::cout << texAlbedo << std::endl;
}

GLuint TextureDataManager::defaultEmission() {
	if (texEmission == 0) {
		float pixels[] = { 0.0f, 0.0f, 0.0f };
		texEmission = createTexture(pixels, GL_RGB);
	}
	return texEmission;
	//	std::cout << texAlbedo << std::endl;
}