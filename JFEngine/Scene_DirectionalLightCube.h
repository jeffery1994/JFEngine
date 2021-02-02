///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#pragma once
#include "Scene.h"

class FrameBuffer;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class Scene_DirectionalLightCube : public Scene
{
public:
	Scene_DirectionalLightCube(float _width, float _height, Camera* _camera);
	void Init();
	void Render();
	void ShadowPass(FrameBuffer* _shadowBuffer);
	void NormalPass(FrameBuffer* _frameBuffer, FrameBuffer* _shadowBuffer = nullptr);
	void PostProcessPass(FrameBuffer* _frameBuffer);
	void BloomPass(FrameBuffer* _pingPongBuffer1, FrameBuffer* _pingPongBuffer2, unsigned int texture);
	void DeferedPass(FrameBuffer* _frameBuffer, FrameBuffer* _shadowBuffer = nullptr);
	void PresentToScreen(Shader* _shader, unsigned int _texture);
	void DeferedFinalPass(Shader* _shader, unsigned int _screenQuadVBO, unsigned int _positionTexture, unsigned int _normalTexture, unsigned int _albedoSpecTexture, FrameBuffer* _frameBuffer = nullptr);
	void DeferedFinalPass(Shader* _shader, unsigned int _screenQuadVBO, unsigned int _positionTexture, unsigned int _normalTexture, unsigned int _albedoTexture, unsigned int _specTexture, FrameBuffer* _frameBuffer = nullptr);

private:
	void SetGeneralShaderConstants(Shader* _shader);
	void UpdateShaderConstants(Shader* _shader);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class SceneFactory_DirectionalLightCube : public SceneFactory
{
public:
	SceneFactory_DirectionalLightCube() {}
	virtual Scene* CreateScene(float _width, float _height, Camera* camera);
};
