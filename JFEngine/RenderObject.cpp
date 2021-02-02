///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#include "RenderObject.h"
#include "Shader.h"
#include <glm/gtc/matrix_transform.hpp> 

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RenderObject::RenderObject()
	:drawOutline(false)
{
	ResetMetrix();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RenderObject::RenderObject(const char* vertexPath, const char* fragmentPath)
	:drawOutline(false)
{
	shader = unique_ptr<Shader>(new Shader(vertexPath, fragmentPath));
	ResetMetrix();
}

RenderObject::RenderObject(const char* vertexPath, const char* fragmentPath, const char* outlineVertPath, const char* outlineFragPath)
	:drawOutline(false)
{
	shader = unique_ptr<Shader>(new Shader(vertexPath, fragmentPath));
	if (outlineVertPath != "" && outlineFragPath != "")
	{
		outlineShader = unique_ptr<Shader>(new Shader(outlineVertPath, outlineFragPath));
	}
	ResetMetrix();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RenderObject::PreRender()
{
	if (shader != nullptr)
	{
		//Pass in transformMetrix
		shader->use();
		unsigned int loc = glGetUniformLocation(shader->ID, "model");
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(ModelMetrix));

		//Pass in view matrix
		loc = glGetUniformLocation(shader->ID, "view");
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(ViewMetrix));

		//Pass in perspective matrix
		loc = glGetUniformLocation(shader->ID, "projection");
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(ProjectionMetrix));
	}

	if (outlineShader != nullptr)
	{
		outlineShader->use();
		unsigned int loc = glGetUniformLocation(outlineShader->ID, "model");
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(ModelMetrix));
		loc = glGetUniformLocation(outlineShader->ID, "view");
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(ViewMetrix));
		loc = glGetUniformLocation(outlineShader->ID, "projection");
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(ProjectionMetrix));
	}

	if (deferedShader != nullptr)
	{
		deferedShader->use();
		unsigned int loc = glGetUniformLocation(deferedShader->ID, "model");
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(ModelMetrix));
		loc = glGetUniformLocation(deferedShader->ID, "view");
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(ViewMetrix));
		loc = glGetUniformLocation(deferedShader->ID, "projection");
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(ProjectionMetrix));
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RenderObject::Render()
{
	if (drawOutline)
	{
		DrawWithOutline();
	}
	else
	{
		Draw();
	}
}

void RenderObject::Render(Shader* _shader)
{
	if (drawOutline)
	{
		DrawWithOutline();
	}
	else
	{
		Draw(_shader);
	}
}

void RenderObject::ShadowPass()
{
	Draw(shadowShader.get());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RenderObject::Destroy()
{
	if (shader == nullptr) return;

	//Apparently we are using unique ptr for managing model and shader
	//for now we do not have to manually delete these pointer
	//Later maybe more member variables will required special treatment.
	shader.reset();
}

void RenderObject::Draw()
{
}

void RenderObject::Draw(Shader* _shader)
{
	if (_shader == nullptr) return;
}

void RenderObject::DrawWithOutline()
{
}

void RenderObject::Rotate(float Degrees, glm::vec3 RotateAxis)
{
	ModelMetrix = glm::rotate(ModelMetrix, glm::radians(Degrees), RotateAxis);
}

void RenderObject::Scale(glm::vec3 scale)
{
	ModelMetrix = glm::scale(ModelMetrix, scale);
}

void RenderObject::Translate(glm::vec3 translate)
{
	ModelMetrix = glm::translate(ModelMetrix, translate);
}

bool RenderObject::AddTexture(Texture& texture)
{
	return false;
}

void RenderObject::ResetMetrix()
{
	SetModelMetrix(glm::mat4(1.0f));
	SetViewMetrix(glm::mat4(1.0f));
	SetProjectionMetrix(glm::mat4(1.0f));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool RenderObject::SetupShader(const char* vertexShaderPath, const char* fragmentShaderPath)
{
	shader = unique_ptr<Shader>(new Shader(vertexShaderPath, fragmentShaderPath));
	if (!shader->GetInited()) return false;
	return true;
}

bool RenderObject::SetupShadowShader(const char* _shadowVertShader, const char* _shadowFrag)
{
	shadowShader = unique_ptr<Shader>(new Shader(_shadowVertShader, _shadowFrag));
	if (!shadowShader->GetInited()) return false;
	return true;
}

bool RenderObject::SetupDeferedShader(const char* _deferedVertShader, const char* _deferedFragShader)
{
	deferedShader = unique_ptr<Shader>(new Shader(_deferedVertShader, _deferedFragShader));
	if (!deferedShader->GetInited()) return false;
	return false;
}
