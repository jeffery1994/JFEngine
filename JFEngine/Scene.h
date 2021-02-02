///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#pragma once
#include <vector>
#include <memory>
#include "RenderObject.h"
#include "Light.h"

class RenderObject;
class Camera;
class FrameBuffer;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class Scene
{
public:
	Scene() {}
	Scene(float Width, float Height, Camera* camera = nullptr);
	virtual void Deinit();
	virtual void Render();
	virtual void Render(FrameBuffer* _frameBuffer);
	virtual void ShadowPass(FrameBuffer* _shadowBuffer);
	virtual void NormalPass(FrameBuffer* _frameBuffer, FrameBuffer* _shadowBuffer = nullptr);
	virtual void BloomPass(FrameBuffer* _pingPongBuffer1, FrameBuffer* _pingPongBuffer2, unsigned int texture) {}
	virtual void DeferedPass(FrameBuffer* _frameBuffer, FrameBuffer* _shadowBuffer = nullptr) {}
	virtual void DeferedFinalPass(Shader* _shader, unsigned int _screenQuadVBO, unsigned int _positionTexture, unsigned int _normalTexture, unsigned int _albedoSpecTexture, FrameBuffer* _frameBuffer = nullptr){}
	virtual void DeferedFinalPass(Shader* _shader, unsigned int _screenQuadVBO, unsigned int _positionTexture, unsigned int _normalTexture, unsigned int _albedoTexture, unsigned int _specTexture, FrameBuffer* _frameBuffer = nullptr) {}
	virtual void PresentToScreen(Shader* _shader, unsigned int _texture){}
	virtual void PresentToScreen(FrameBuffer* _frameBuffer, Shader* _shader, unsigned int _texture);
	virtual void PostProcessPass(FrameBuffer* _frameBuffer) {}
	virtual void Init() = 0;
protected:
	std::vector<std::unique_ptr<RenderObject>> renderObjects;
	std::vector<std::unique_ptr<Light>> Lights;
	glm::mat4 lightSpaceMatrix;
	Camera* camera;
	float width, height;
private:
	
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class SceneFactory
{
public:
	SceneFactory() {}
	virtual Scene* CreateScene(float _width, float _height, Camera* _camera) = 0;
};