///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#pragma once
#include <vector>

class Mesh;
class Shader;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
enum POST_PROCESS_EFFECT
{
	NO_EFFECT,
	INVERSION,
	GRAY_SCALE,
	GRAY_SCALE_ADVANCE,
	SHARPEN,
	BLUR,
	EDGE_DETECTION,
	SHADOW_MAP,
	HDR,
	GAUSSIAN_BLUR,
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
enum FRAME_BUFFER_TYPE
{
	POST_PROCESS,
	SHADOW,
	PING_PONG_ONE,
	PING_PONG_TWO,
	HDR_TO_CUBEMAP,
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class FrameBuffer
{
public:
	FrameBuffer(float _width, float _height, FRAME_BUFFER_TYPE _type);
	FrameBuffer(float _width, float _height, int _numOfColorBuffers);
	~FrameBuffer();
	void InitPostProcessBuffer(unsigned int _numOfColorBuffer = 1);
	void InitShadowBuffer();
	void InitHDRToCubeMapBuffer();
	void Deinit();
	void Draw();
	void DrawWithShaderAndTexture(Shader* _shader, unsigned int _textureObject);
	void DoGaussianBlur(unsigned int _textureObject, bool _horizontal);
	void DrawBloomResult(unsigned int _gaussianBlurTexture, unsigned int _normalTexture);
	inline unsigned int GetFrameBuffer() { return fbo; }
	inline unsigned int GetColorBufferByIndex(int _index) { return colorBuffers[_index]; }
	const inline POST_PROCESS_EFFECT GetPostProcessEffect() { return postProcessEffect; }
	const inline FRAME_BUFFER_TYPE GetFrameBufferType() { return this->frameBufferType; }
	const inline float GetShadowMapWidth() { return this->shadowWidth; }
	const inline float GetShaodwMapHeight() { return this->shadowHeight; }
	const inline unsigned int GetShadowMap() { return this->depthMap; }
	inline Shader* GetProcessShader() { return this->postProcessShader; }
	void SetPostProcessEffect(POST_PROCESS_EFFECT _newEffect) { postProcessEffect = _newEffect; }
private:
	void SetupScreenQuad();
private:
	//std::vector<unsigned int> colorBuffers;
	//unsigned int colorBuffers[2];
	std::vector<unsigned int> colorBuffers;
	unsigned int* attachments;
	Shader* postProcessShader;
	Shader* GaussianBlurShader;
	Shader* bloomShader;
	POST_PROCESS_EFFECT postProcessEffect;
	FRAME_BUFFER_TYPE frameBufferType;
	float width;
	float height;
	float shadowWidth = 1024;
	float shadowHeight = 1024;
	unsigned int fbo;
	//unsigned int texColorBuffer;
	
	unsigned int depthMap;
	unsigned int rbo;
	unsigned int quadVAO;
	unsigned int quadVBO;
	bool Inited;
};