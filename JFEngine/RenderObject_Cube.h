///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#pragma once
#include "RenderObject.h"
#include "Mesh.h"

class RenderObject_Cube : public RenderObject
{
public:
	RenderObject_Cube();
	RenderObject_Cube(const char* vertexPath, const char* fragmentPath, const char* outlineVertex = "", const char* outlineFrag = "");
	void SetupRenderObject(const char* modelPath = "");

	void Destroy();
	void Draw();
	void DrawWithOutline();
	void Draw(Shader* _shader);

	bool AddTexture(Texture& texture);
	bool AddCubeMapTexture(std::vector<Texture>& _texture);

	const unsigned int GetNumberOfTexture();
private:
	std::unique_ptr<Mesh> mesh;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class RenderObjectFactory_Cube : public RenderObjectFactory
{
public:
	RenderObjectFactory_Cube()
		:RenderObjectFactory()
	{

	}
	RenderObject* CreateRenderObject();
	RenderObject* CreateRenderObject(const char* vertexPath, const char* fragmentPath);
	RenderObject* CreateRenderObject(const char* vertexPath, const char* fragmentPath, const char* outlinePath, const char* outlineFrag);
};
