#pragma once
#include "../Data/TextureData.h"
#include <map>

class TextureDataManager {
private:
	static std::map<std::string, TextureData*> textureDataMap;
	static GLuint texAlbedo;
	static GLuint texRoughness;
	static GLuint texMetalness;
	static GLuint texNormal;
	static GLuint texEmission;

	static GLuint createTexture(float pixels[], GLenum channelFormat);

public:
	TextureDataManager() = default;
	~TextureDataManager() = default;
	static TextureData* getTextureData(std::string textureName);
	static TextureData* getImageData(std::string imageName);
	static TextureData* getCubemapData(std::string folder, std::vector<std::string> faces);
	static GLuint defaultAlbedo();
	static GLuint defaultRoughness();
	static GLuint defaultMetalness();
	static GLuint defaultNormal();
	static GLuint defaultEmission();
};
