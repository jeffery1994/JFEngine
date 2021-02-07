///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#include "Scene.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include "camera.h"
#include "FrameBuffer.h"
#include "TextureManager.h"
#include "ShaderManager.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Scene::Scene(float Width, float Height, Camera* _camera)
	:width(Width)
	,height(Height)
	,camera(_camera)
{
}

Scene::Scene(float _width, float _height, Camera* _camera, TextureManager* _textureManager, ShaderManager* _shaderManager)
	:width(_width)
	,height(_height)
	,camera(_camera)
	,textureManager(_textureManager)
	,shaderManager(_shaderManager)
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Scene::Deinit()
{
	for (auto idx = 0; idx < renderObjects.size(); ++idx)
	{
		renderObjects[idx].get()->Destroy();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Scene::Render()
{
	glm::mat4 projection = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);
	if (camera != nullptr)
	{
		projection = glm::perspective(glm::radians(camera->Zoom), (float)width / (float)height, 0.1f, 100.0f);
		view = camera->GetViewMatrix();
	}

	for (auto idx = 0; idx < renderObjects.size(); ++idx)
	{
		renderObjects[idx].get()->SetProjectionMetrix(projection);
		renderObjects[idx].get()->SetViewMetrix(view);
		renderObjects[idx].get()->PreRender();
		renderObjects[idx].get()->Render();
		renderObjects[idx].get()->PostRender();
	}
}

void Scene::Render(FrameBuffer* _frameBuffer)
{
	if (_frameBuffer == nullptr) return;
	glBindFramebuffer(GL_FRAMEBUFFER, _frameBuffer->GetFrameBuffer());

	//Clear first
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	//Render to texture
	Render();

	//Render texture to frame buffer
	_frameBuffer->SetPostProcessEffect(NO_EFFECT);
	_frameBuffer->Draw();
}

void Scene::ShadowPass(FrameBuffer* _frameBuffer)
{
	if (_frameBuffer == nullptr) return;
	glBindFramebuffer(GL_FRAMEBUFFER, _frameBuffer->GetFrameBuffer());

	//Clear first
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	for (auto idx = 0; idx < renderObjects.size(); ++idx)
	{
		renderObjects[idx].get()->GetShadowShader()->use();
		renderObjects[idx].get()->GetShadowShader()->setMat4("model", renderObjects[idx].get()->GetModelMetrix());
		renderObjects[idx].get()->GetShadowShader()->setMat4("lightSpaceMatrix", lightSpaceMatrix);
		renderObjects[idx].get()->ShadowPass();
	}
}

void Scene::NormalPass(FrameBuffer* _frameBuffer, FrameBuffer* _shadowBuffer)
{
}

void Scene::PresentToScreen(FrameBuffer* _frameBuffer, Shader* _shader, unsigned int _texture)
{
	if (_frameBuffer == nullptr) return;
	if (_shader == nullptr) return;
	_frameBuffer->DrawWithShaderAndTexture(_shader, _texture);
}
