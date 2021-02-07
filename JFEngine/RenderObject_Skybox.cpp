///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#include "RenderObject_Skybox.h"
#include "TextureManager.h"
#include "Texture.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RenderObject_Skybox::RenderObject_Skybox()
{
}

void RenderObject_Skybox::Draw(Shader* _shader)
{
	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
	mesh->Draw();
	glDepthFunc(GL_LESS); // set depth function back to default
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RenderObject_Skybox::RenderObject_Skybox(const char* vertexPath, const char* fragmentPath, const char* outlineVertex, const char* outlineFrag)
	:RenderObject(vertexPath, fragmentPath, outlineVertex, outlineFrag)
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RenderObject_Skybox::SetupRenderObject(const char* modelPath)
{
	mesh = unique_ptr<Mesh>(new Mesh(SIMPLE_MESH_TYPE::SKYBOX));
}

void RenderObject_Skybox::PreRender()
{
	//Pass in transformMetrix
	shader->use();
	unsigned int loc = glGetUniformLocation(shader->ID, "model");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(ModelMetrix));

	//Pass in view matrix
	loc = glGetUniformLocation(shader->ID, "view");
	ViewMetrix = glm::mat4(glm::mat3(ViewMetrix)); // remove translation from the view matrix
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(ViewMetrix));

	//Pass in perspective matrix
	loc = glGetUniformLocation(shader->ID, "projection");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(ProjectionMetrix));
}

void RenderObject_Skybox::PreRender(Shader* _shader)
{
	if (_shader == nullptr) return;
	_shader->use();
	unsigned int loc = glGetUniformLocation(_shader->ID, "model");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(ModelMetrix));
	loc = glGetUniformLocation(_shader->ID, "view");
	ViewMetrix = glm::mat4(glm::mat3(ViewMetrix)); // remove translation from the view matrix
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(ViewMetrix));
	loc = glGetUniformLocation(_shader->ID, "projection");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(ProjectionMetrix));

	for (auto idx = 0; idx < textureReferences.size(); ++idx)
	{
		if (textureReferences[idx] == nullptr) continue;
		glActiveTexture(GL_TEXTURE0 + idx); // active proper texture unit before binding
		glUniform1i(glGetUniformLocation(_shader->ID, textureReferences[idx]->GetName().c_str()), idx);
		switch (textureReferences[idx]->GetTextureType())
		{
		case TEXTURE_TYPE::TEXTURE_2D:
			glBindTexture(GL_TEXTURE_2D, textureReferences[idx]->GetTexture());
			break;
		case TEXTURE_TYPE::TEXTURE_3D:
			break;
		case TEXTURE_TYPE::TEXTURE_CUBEMAP:
			glBindTexture(GL_TEXTURE_CUBE_MAP, textureReferences[idx]->GetTexture());
			break;
		case TEXTURE_TYPE::TEXTURE_HDR:
			break;
		default:
			break;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RenderObject_Skybox::Destroy()
{
	shader.reset();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RenderObject_Skybox::Draw()
{
	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
	shader->use();
	mesh->Draw(shader.get());
	glDepthFunc(GL_LESS); // set depth function back to default
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RenderObject_Skybox::DrawWithOutline()
{
}

bool RenderObject_Skybox::AddCubeMapTexture(std::vector<Texture>& _texture)
{
	return mesh.get()->AddCubeMapTexture(_texture);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RenderObject* RenderObjectFactory_Skybox::CreateRenderObject()
{
	RenderObject* object = new RenderObject_Skybox();
	object->SetupRenderObject();
	return object;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RenderObject* RenderObjectFactory_Skybox::CreateRenderObject(const char* vertexPath, const char* fragmentPath)
{
	RenderObject* object = new RenderObject_Skybox(vertexPath, fragmentPath);
	object->SetupRenderObject();
	return object;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RenderObject* RenderObjectFactory_Skybox::CreateRenderObject(const char* vertexPath, const char* fragmentPath, const char* outlinePath, const char* outlineFrag)
{
	RenderObject* object = new RenderObject_Skybox(vertexPath, fragmentPath, outlinePath, outlineFrag);
	object->SetupRenderObject();
	return object;
}
