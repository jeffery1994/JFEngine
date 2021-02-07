#include "Texture.h"
#include <glad/glad.h>
#include "stb_image.h"
#include <iostream>

static const unsigned int NUM_OF_CUBEMAP_FACES = 6;

JFTexture::JFTexture(std::string _path, TEXTURE_TYPE _type, std::string _name)
	:path(_path)
	,textureType(_type)
	,name(_name)
{
	Setup();
}

JFTexture::JFTexture(const std::vector<std::string> _cubemapPaths, std::string _name)
	:cubeMapPaths(_cubemapPaths)
	,textureType(TEXTURE_TYPE::TEXTURE_CUBEMAP)
	,name(_name)
{
	Setup();
}

JFTexture::JFTexture(const TEXTURE_TYPE _type, const std::string _name)
	:textureType(_type)
	,name(_name)
{
	SetupEmptyTexture();
}

JFTexture::~JFTexture()
{
}

void JFTexture::Setup()
{
	switch (textureType)
	{
	case TEXTURE_TYPE::TEXTURE_2D:
		SetupTexture2D();
		break;
	case TEXTURE_TYPE::TEXTURE_CUBEMAP:
		SetupTextureCubemap();
		break;
	case TEXTURE_TYPE::TEXTURE_HDR:
		SetupTextureHDR();
		break;
	case TEXTURE_TYPE::TEXTURE_3D:
		SetupTexture3D();
		break;
	default:
		break;
	}
}

void JFTexture::SetupEmptyTexture()
{
	switch (textureType)
	{
	case TEXTURE_TYPE::TEXTURE_2D:
		break;
	case TEXTURE_TYPE::TEXTURE_CUBEMAP:
		SetupEmptyCubemap();
		break;
	case TEXTURE_TYPE::TEXTURE_HDR:
		break;
	case TEXTURE_TYPE::TEXTURE_3D:
		break;
	default:
		break;
	}
}

void JFTexture::SetupTexture2D()
{
	const char* _path = path.c_str();
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	// tell stb_image.h to flip loaded texture's on the y-axis.
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(_path, &width, &height, &nrChannels, 0);
	if (data)
	{
		if (nrChannels == 3)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
		return;
	}
	stbi_image_free(data);
	return;
}

void JFTexture::SetupTextureCubemap()
{
	if (cubeMapPaths.size() != NUM_OF_CUBEMAP_FACES) return;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);
	int width, height, nrChannels;
	for (auto idx = 0; idx < NUM_OF_CUBEMAP_FACES; ++idx)
	{
		unsigned char* data = stbi_load(cubeMapPaths[idx].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + idx,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap tex failed to load at path: " << cubeMapPaths[idx]<< std::endl;
			stbi_image_free(data);
			return;
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	return;
}

void JFTexture::SetupTextureHDR()
{
	int width, height, nrComponents;
	float* data = stbi_loadf(path.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Failed to load HDR image." << std::endl;
	}
}

void JFTexture::SetupTexture3D()
{

}

void JFTexture::SetupEmptyCubemap()
{
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);
	for (unsigned int i = 0; i < NUM_OF_CUBEMAP_FACES; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}
