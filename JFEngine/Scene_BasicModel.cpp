///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#include "Scene_BasicModel.h"
#include "RenderObject_Model.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Scene_BasicModel::Scene_BasicModel(float _width, float _height, Camera* _camera)
	:Scene(_width, _height, _camera)
{
	Init();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Scene_BasicModel::Init()
{
	RenderObjectFactory_Model* factory = new RenderObjectFactory_Model();
	renderObjects.push_back(unique_ptr<RenderObject>(factory->CreateRenderObject("ModelLoading.vert", "ModelLoading.frag")));
	renderObjects[renderObjects.size()-1].get()->SetupRenderObject("../resource/backpack.obj");
	delete factory;
	glEnable(GL_DEPTH_TEST);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Scene* SceneFactory_BasicModel::CreateScene(float _width, float _height, Camera* _camera)
{
	return new Scene_BasicModel(_width, _height, _camera);
}