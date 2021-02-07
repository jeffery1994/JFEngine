///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#pragma once
#include "Scene.h"

class FrameBuffer;
class Shader;
class JFTexture;
class RenderObject;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class Scene_HDRToCubemap : public Scene
{
public:
	Scene_HDRToCubemap(float _width, float _height, Camera* _camera);
	Scene_HDRToCubemap(float _width, float _height, Camera* _camera, TextureManager* _textureManager, ShaderManager* _shaderManager);

	void Init();
	void Render();
	void PreComputingPass(FrameBuffer* _frameBuffer = nullptr);
private:
	std::vector<glm::mat4> captureViews;
	glm::mat4 captureProjection;
	Shader* hdrToCubemapShader = nullptr;
	JFTexture* enironmentCubemap = nullptr;
	RenderObject* envMapCube = nullptr;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class SceneFactory_HDRToCubemap : public SceneFactory
{
public:
	SceneFactory_HDRToCubemap() {}
	virtual Scene* CreateScene(float _width, float _height, Camera* _camera);
	virtual Scene* CreateScene(float _width, float _height, Camera* _camera, TextureManager* _textureManager, ShaderManager* _shaderManager);
};
