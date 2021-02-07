#include "TextureManager.h"
#include "Texture.h"

TextureManager::~TextureManager()
{
	Clear();
}

JFTexture* TextureManager::GetTextureByName(const std::string _name)
{
	for (auto texture : textures)
	{
		if (texture->GetName() == _name) return texture;
	}
	return nullptr;
}

void TextureManager::AddTexture(const std::string _path, TEXTURE_TYPE _textureType, std::string _name)
{
	textures.push_back(new JFTexture(_path, _textureType, _name));
}

void TextureManager::AddTexture(const std::vector<std::string> _cubemapPaths, std::string _name)
{
	textures.push_back(new JFTexture(_cubemapPaths, _name));
}

void TextureManager::CreateEmptyTexture(TEXTURE_TYPE _textureType, std::string _name)
{
	textures.push_back(new JFTexture(_textureType, _name));
}

void TextureManager::Clear()
{
	for (auto texture : textures)
	{
		delete texture;
	}
	textures.clear();
}
