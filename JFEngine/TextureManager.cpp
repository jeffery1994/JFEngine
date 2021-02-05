#include "TextureManager.h"
#include "Texture.h"

TextureManager::~TextureManager()
{
	Clear();
}

void TextureManager::AddTexture(JFTexture* _texture)
{
	textures.push_back(_texture);
}

void TextureManager::AddTexture(const std::string _path, TEXTURE_TYPE _textureType)
{
	textures.push_back(new JFTexture(_path, _textureType));
}

void TextureManager::AddTexture(const std::vector<std::string> _cubemapPaths)
{
	textures.push_back(new JFTexture(_cubemapPaths));
}

void TextureManager::Clear()
{
	for (auto texture : textures)
	{
		delete texture;
	}
	textures.clear();
}
