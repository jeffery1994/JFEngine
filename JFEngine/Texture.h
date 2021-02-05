#pragma once
#include <string>
#include <vector>

enum TEXTURE_TYPE
{
	TEXTURE_2D,
	TEXTURE_CUBEMAP,
	TEXTURE_HDR,
	TEXTURE_3D
};

class JFTexture
{
public:
	JFTexture(std::string _path, TEXTURE_TYPE _type);
	JFTexture(const std::vector<std::string> _cubemapPaths);
	~JFTexture();

	virtual void Setup();
	void SetupTexture2D();
	void SetupTextureCubemap();
	void SetupTextureHDR();
	void SetupTexture3D();

	//Getter
	const std::string GetCubeMapPathByIndex(const unsigned int& _index)
	{
		std::string temp = "";
		if (_index < 0 || _index > cubeMapPaths.size()) return temp;
		return cubeMapPaths[_index];
	}
	const std::string GetPath() { return this->path; }
	const unsigned int GetTexture() { return this->id; }
	const TEXTURE_TYPE GetTextureType() { return this->textureType; }

	//Setter
	void SetTextureType(const TEXTURE_TYPE _textureType) { this->textureType = _textureType; }
protected:
	std::vector<std::string> cubeMapPaths;
	std::string path;
	unsigned int id;
	TEXTURE_TYPE textureType;
};