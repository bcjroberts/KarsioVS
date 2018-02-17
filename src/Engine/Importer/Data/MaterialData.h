// This has all the material data for a model
// aka shader and texture data
//

#ifndef PROJECTKARSIO_MATERIALDATA_H
#define PROJECTKARSIO_MATERIALDATA_H
#include "ShaderData.h"
#include "TextureData.h"

class MaterialData {

public:
	ShaderData shaderData;
//	GLuint shaderDataID;
	struct Texture {
		TextureData albedo;
		TextureData roughness;
		TextureData texture1;
		TextureData texture2;
		TextureData texture3;
	};
	void addShader(std::string path, GLenum type);
	void linkShader();
	void addTexture();
	void clearShader();
	MaterialData();
	~MaterialData() = default;
};


#endif //PROJECTKARSIO_MATERIALDATA_H
