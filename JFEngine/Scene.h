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
class TextureManager;
class ShaderManager;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class Scene
{
public:
	Scene() {}
	Scene(float _width, float _height, Camera* _camera = nullptr);
	Scene(float _width, float _height, Camera* _camera, TextureManager* _textureManager, ShaderManager* _shaderManager);
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
	virtual void DirectPBRPass(FrameBuffer* _frameBuffer, Shader* _directPBRShader) {}
	virtual void PreComputingPass(FrameBuffer* _frameBuffer = nullptr) {}
	virtual void Init() = 0;
protected:
	std::vector<std::unique_ptr<RenderObject>> renderObjects;
	std::vector<std::unique_ptr<Light>> Lights;
	glm::mat4 lightSpaceMatrix;
	TextureManager* textureManager;
	ShaderManager* shaderManager;
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
	virtual Scene* CreateScene(float _width, float _height, Camera* _camera, TextureManager* _textureManager, ShaderManager* _shaderManager)
	{
		return nullptr;
	}
};