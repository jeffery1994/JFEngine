#include "Scene_HDRToCubemap.h"
#include "RenderObject_Skybox.h"
#include "RenderObject_Cube.h"
#include "camera.h"
#include "ResourceData.def"
#include "TextureManager.h"
#include "ShaderManager.h"
#include "Shader.h"
#include "FrameBuffer.h"
#include "Shader.h"
#include "Texture.h"

Scene_HDRToCubemap::Scene_HDRToCubemap(float _width, float _height, Camera* _camera)
    :Scene(_width, _height, _camera)
{
    Init();
	captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	captureViews.push_back(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	captureViews.push_back(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	captureViews.push_back(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
	captureViews.push_back(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
	captureViews.push_back(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	captureViews.push_back(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
}

Scene_HDRToCubemap::Scene_HDRToCubemap(float _width, float _height, Camera* _camera, TextureManager* _textureManager, ShaderManager* _shaderManager)
	:Scene(_width, _height, _camera, _textureManager, _shaderManager)
{
	Init();
	captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	captureViews.push_back(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	captureViews.push_back(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	captureViews.push_back(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
	captureViews.push_back(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
	captureViews.push_back(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	captureViews.push_back(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
}

void Scene_HDRToCubemap::Init()
{
	if (textureManager == nullptr || shaderManager == nullptr) return;

	JFTexture* skyboxTex = textureManager->GetTextureByName("skybox");
	JFTexture* hdrTex = textureManager->GetTextureByName("equirectangularMap");
	Shader* skyboxShader = shaderManager->GetShaderByType(SHADER_TYPE::SHADER_SKYBOX);
	hdrToCubemapShader = shaderManager->GetShaderByType(SHADER_TYPE::SHADER_HDR_TO_CUBEMAP);
	enironmentCubemap = textureManager->GetTextureByName("skybox");

	//Cube
	RenderObjectFactory_Cube* cubeFactory = new RenderObjectFactory_Cube();
	envMapCube = cubeFactory->CreateRenderObject();
	envMapCube->AddTextureReference(hdrTex);
	envMapCube->SetShaderReference(hdrToCubemapShader);
	delete cubeFactory;

	//Skybox
	RenderObjectFactory_Skybox* factory = new RenderObjectFactory_Skybox();
	renderObjects.push_back(unique_ptr<RenderObject>(factory->CreateRenderObject()));
	if (skyboxTex != nullptr)
	{
		renderObjects[renderObjects.size() - 1].get()->AddTextureReference(enironmentCubemap);
	}
	if (skyboxShader != nullptr)
	{
		renderObjects[renderObjects.size() - 1].get()->SetShaderReference(skyboxShader);
	}
	renderObjects[renderObjects.size() - 1].get()->Translate(camera->Position);
	delete factory;

	//Config ogl global state
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
}

void Scene_HDRToCubemap::Render()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
		renderObjects[idx].get()->PreRender(renderObjects[idx].get()->GetShaderReference());
		renderObjects[idx].get()->Render(renderObjects[idx].get()->GetShaderReference());
		renderObjects[idx].get()->PostRender();
	}
}

void Scene_HDRToCubemap::PreComputingPass(FrameBuffer* _frameBuffer)
{
	if (_frameBuffer == nullptr
		|| hdrToCubemapShader == nullptr
		|| enironmentCubemap == nullptr
		|| envMapCube == nullptr
		) 
		return;
	glBindFramebuffer(GL_FRAMEBUFFER, _frameBuffer->GetFrameBuffer());
	glm::mat4 projection = captureProjection;
	

	for (unsigned int i = 0; i < 6; ++i)
	{
		glm::mat4 view = captureViews[i];
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, enironmentCubemap->GetTexture(), 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		envMapCube->SetProjectionMetrix(projection);
		envMapCube->SetViewMetrix(view);
		envMapCube->PreRender(envMapCube->GetShaderReference());
		envMapCube->Render(envMapCube->GetShaderReference());
		envMapCube->PostRender();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Scene* SceneFactory_HDRToCubemap::CreateScene(float _width, float _height, Camera* _camera)
{
	return new Scene_HDRToCubemap(_width, _height, _camera);
}

Scene* SceneFactory_HDRToCubemap::CreateScene(float _width, float _height, Camera* _camera, TextureManager* _textureManager, ShaderManager* _shaderManager)
{
	return new Scene_HDRToCubemap(_width, _height, _camera, _textureManager, _shaderManager);
}
