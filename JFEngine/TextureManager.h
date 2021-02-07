#pragma once
#include <vector>

enum TEXTURE_TYPE;
class JFTexture;

class TextureManager
{
public:
	TextureManager() {}
	~TextureManager();

	//Getter
	JFTexture* GetTextureByIndex(const unsigned int& _index) { return textures[_index]; }
	JFTexture* GetTextureByName(const std::string _name);
	const unsigned int GetNumberOfTextures() { return (const unsigned int)textures.size(); }

	//Add textures
	void AddTexture(const std::string _path, TEXTURE_TYPE _textureType, std::string _name);
	void AddTexture(const std::vector<std::string> _cubemapPaths, std::string _name);
	void CreateEmptyTexture(TEXTURE_TYPE _textureType, std::string _name);
	void Clear();

private:
	std::vector<JFTexture*> textures;
};
