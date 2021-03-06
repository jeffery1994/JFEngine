///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#include "RenderObject_Model.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RenderObject_Model::RenderObject_Model()
{

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RenderObject_Model::RenderObject_Model(const char* vertexPath, const char* fragmentPath, const char* modelPath)
	:RenderObject(vertexPath, fragmentPath)
{
	model = unique_ptr<Model>(new Model(modelPath));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RenderObject_Model::RenderObject_Model(const char* vertexPath, const char* fragmentPath)
	:RenderObject(vertexPath, fragmentPath)
{

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RenderObject_Model::SetupRenderObject(const char* modelPath)
{
	if (modelPath == "") return;
	model = unique_ptr<Model>(new Model(modelPath));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RenderObject_Model::Render()
{
	if (shader == nullptr || model == nullptr ) return;
	shader->use();
	model->Draw(shader.get());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RenderObject_Model::Destroy()
{
	if (shader == nullptr || model == nullptr) return;
	//Apparently we are using unique ptr for managing model and shader
	//for now we do not have to manually delete these pointer
	//Later maybe more member variables will required special treatment.
	shader.reset();
	model.reset();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RenderObject* RenderObjectFactory_Model::CreateRenderObject()
{
	return new RenderObject_Model();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RenderObject* RenderObjectFactory_Model::CreateRenderObject(const char* vertexPath, const char* fragmentPath)
{
	return new RenderObject_Model(vertexPath, fragmentPath);
}