///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#include "RenderObject_Quad.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RenderObject_Quad::RenderObject_Quad()
{

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RenderObject_Quad::RenderObject_Quad(const char* vertexPath, const char* fragmentPath, const char* outlineVertex, const char* outlineFrag)
	:RenderObject(vertexPath, fragmentPath, outlineVertex, outlineFrag)
{

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RenderObject_Quad::Destroy()
{
	shader.reset();
}

void RenderObject_Quad::Draw()
{
	shader->use();
	mesh->Draw(shader.get());
}

void RenderObject_Quad::Draw(Shader* _shader)
{
	_shader->use();
	mesh->Draw(_shader);
}

void RenderObject_Quad::DrawWithOutline()
{
	// 1st. render pass, draw objects as normal, writing to the stencil buffer
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);
	shader->use();
	mesh->Draw(shader.get());

	// 2nd. render pass: now draw slightly scaled versions of the objects, this time disabling stencil writing.
	// Because the stencil buffer is now filled with several 1s. The parts of the buffer that are 1 are not drawn, thus only drawing 
	// the objects' size differences, making it look like borders.
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00);
	glDisable(GL_DEPTH_TEST);//Why disable depth test?
	outlineShader->use();
	float scale = 1.1;

	//Set model metrix to unit metrix
	ModelMetrix = glm::mat4(1.0);

	//Scale a little bit bigger
	ModelMetrix = glm::scale(ModelMetrix, glm::vec3(scale, scale, scale));

	//pass new model metrix to shader
	outlineShader->setMat4("model", ModelMetrix);
	mesh->Draw(outlineShader.get());

	//Reset model metrix
	ModelMetrix = glm::mat4(1.0);

	//Re-enable wrting to stencil buffer
	glStencilMask(0xFF);
	glStencilFunc(GL_ALWAYS, 0, 0xFF);
	glEnable(GL_DEPTH_TEST);
}

bool RenderObject_Quad::AddTexture(Texture& texture)
{
	return mesh->AddTexture(texture);
}

const unsigned int RenderObject_Quad::GetNumberOfTexture()
{
	return mesh->GetNumberOfTextures();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RenderObject_Quad::SetupRenderObject(const char* modelPath /* = "" */)
{
	mesh = unique_ptr<Mesh>(new Mesh(SIMPLE_MESH_TYPE::QUAD));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RenderObject* RenderObjectFactory_Quad::CreateRenderObject()
{
	RenderObject* object = new RenderObject_Quad();
	object->SetupRenderObject();
	return object;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RenderObject* RenderObjectFactory_Quad::CreateRenderObject(const char* vertexPath, const char* fragmentPath)
{
	RenderObject* object = new RenderObject_Quad(vertexPath, fragmentPath);
	object->SetupRenderObject();
	return object;
}

RenderObject* RenderObjectFactory_Quad::CreateRenderObject(const char* vertexPath, const char* fragmentPath, const char* outlinePath, const char* outlineFrag)
{
	RenderObject* object = new RenderObject_Quad(vertexPath, fragmentPath, outlinePath, outlineFrag);
	object->SetupRenderObject();
	return object;
}
