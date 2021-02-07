///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#pragma once
#include "RenderObject.h"
#include "Mesh.h"

class RenderObject_Skybox : public RenderObject
{
public:
	RenderObject_Skybox();
	//RenderObject_Skybox(const char* _vertexPath, const char* _fragmentPath);
	RenderObject_Skybox(const char* vertexPath, const char* fragmentPath, const char* outlineVertex = "", const char* outlineFrag = "");
	void SetupRenderObject(const char* modelPath = "");

	void PreRender();
	void PreRender(Shader* _shader);
	void Destroy();
	void Draw();
	void Draw(Shader* _shader);
	void DrawWithOutline();
	bool AddCubeMapTexture(std::vector<Texture>& _texture);
private:
	std::unique_ptr<Mesh> mesh;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class RenderObjectFactory_Skybox : public RenderObjectFactory
{
public:
	RenderObjectFactory_Skybox()
		:RenderObjectFactory()
	{

	}
	RenderObject* CreateRenderObject();
	RenderObject* CreateRenderObject(const char* vertexPath, const char* fragmentPath);
	RenderObject* CreateRenderObject(const char* vertexPath, const char* fragmentPath, const char* outlinePath, const char* outlineFrag);
};
