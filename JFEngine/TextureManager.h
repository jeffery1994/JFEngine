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
	const JFTexture* GetTextureByIndex(const unsigned int& _index) { return textures[_index]; }
	const unsigned int GetNumberOfTextures() { return (const unsigned int)textures.size(); }

	//Add textures
	void AddTexture(JFTexture* _texture); //This might cause problems
	void AddTexture(const std::string _path, TEXTURE_TYPE _textureType);
	void AddTexture(const std::vector<std::string> _cubemapPaths);
	void Clear();

private:
	std::vector<JFTexture*> textures;
};
