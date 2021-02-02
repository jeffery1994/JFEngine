///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#pragma once
#include "RenderObject.h"
#include "Mesh.h"

class RenderObject_Quad : public RenderObject
{
public:
	RenderObject_Quad();
	RenderObject_Quad(const char* vertexPath, const char* fragmentPath, const char* outlineVertex = "", const char* outlineFrag = "");
	void SetupRenderObject(const char* modelPath = "");
	void Destroy();
	void Draw();
	void Draw(Shader* _shader);
	void DrawWithOutline();
	bool AddTexture(Texture& texture);
	const unsigned int GetNumberOfTexture();
private:
	std::unique_ptr<Mesh> mesh;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class RenderObjectFactory_Quad : public RenderObjectFactory
{
public:
	RenderObjectFactory_Quad()
		:RenderObjectFactory()
	{

	}
	RenderObject* CreateRenderObject();
	RenderObject* CreateRenderObject(const char* vertexPath, const char* fragmentPath);
	RenderObject* CreateRenderObject(const char* vertexPath, const char* fragmentPath, const char* outlinePath, const char* outlineFrag);
};