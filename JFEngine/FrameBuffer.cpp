///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#include "FrameBuffer.h"
#include <glad/glad.h>
#include <iostream>
#include"Mesh.h"
#include "Shader.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
FrameBuffer::FrameBuffer(float _width, float _height, FRAME_BUFFER_TYPE _type)
	:width(_width)
	,height(_height)
	,Inited(false)
	,postProcessEffect(POST_PROCESS_EFFECT::NO_EFFECT)
	, frameBufferType(_type)
{
	switch (this->frameBufferType)
	{
	case POST_PROCESS:
	case PING_PONG_ONE:
	case PING_PONG_TWO:
		InitPostProcessBuffer();
		break;
	case SHADOW:
		InitShadowBuffer();
		break;
	case HDR_TO_CUBEMAP:
		InitHDRToCubeMapBuffer();
	default:
		break;
	}
	SetupScreenQuad();
}

FrameBuffer::FrameBuffer(float _width, float _height, int _numOfColorBuffers)
	:width(_width)
	, height(_height)
	, Inited(false)
	, postProcessEffect(POST_PROCESS_EFFECT::NO_EFFECT)
{
	InitPostProcessBuffer(_numOfColorBuffers);
	SetupScreenQuad();
	frameBufferType = POST_PROCESS;
}

FrameBuffer::~FrameBuffer()
{
	Deinit();
}

void FrameBuffer::InitPostProcessBuffer(unsigned int _numOfColorBuffer)
{
	if (Inited) return;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	attachments = new unsigned int[_numOfColorBuffer];
	for (unsigned int idx = 0; idx < _numOfColorBuffer; ++idx)
	{
		// generate color texture
		unsigned int colorBuffer;
		colorBuffers.push_back(colorBuffer);
		glGenTextures(1, &colorBuffers[idx]);
		glBindTexture(GL_TEXTURE_2D, colorBuffers[idx]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, (GLsizei)width, (GLsizei)height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
		// attach it to currently bound framebuffer object
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + idx, GL_TEXTURE_2D, colorBuffers[idx], 0);
		attachments[idx] = GL_COLOR_ATTACHMENT0 + idx;
	}
	glDrawBuffers(_numOfColorBuffer, attachments);

	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, (GLsizei)width, (GLsizei)height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//setup post process shader
	postProcessShader = new Shader("PostProcess.vert", "PostProcess.frag");

	Inited = true;
}

void FrameBuffer::InitShadowBuffer()
{
	if (Inited) return;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// Generate depth texture
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, (GLsizei)width, (GLsizei)height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// attach it to currently bound framebuffer object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//setup post process shader
	postProcessShader = new Shader("PostProcess.vert", "PostProcess.frag");

	Inited = true;
}

void FrameBuffer::InitHDRToCubeMapBuffer()
{
	if (Inited) return;
	unsigned int captureRBO;
	glGenFramebuffers(1, &fbo);
	glGenRenderbuffers(1, &captureRBO);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);
	Inited = true;
}

void FrameBuffer::Deinit()
{
	if (!Inited) return;
	delete[] attachments;
	glDeleteFramebuffers(1, &fbo);
	Inited = false;
}

void FrameBuffer::SetupScreenQuad()
{
	static float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
	// positions   // texCoords
	-1.0f,  1.0f,  0.0f, 1.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,

	-1.0f,  1.0f,  0.0f, 1.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	 1.0f,  1.0f,  1.0f, 1.0f
	};

	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	if (postProcessShader != nullptr)
	{
		postProcessShader->use();
		postProcessShader->setInt("screenTexture", 0);
	}
	GaussianBlurShader = new Shader("GaussianBlur.vert", "GaussianBlur.frag");
	bloomShader = new Shader("bloomFinal.vert", "bloomFinal.frag");
}

void FrameBuffer::Draw()
{
	// now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// disable depth test so screen-space quad isn't discarded due to depth test
	glDisable(GL_DEPTH_TEST);

	// clear all relevant buffers
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	postProcessShader->use();
	postProcessShader->setInt("PostProcessEffect", (int)postProcessEffect);
	glBindVertexArray(quadVAO);
	glActiveTexture(GL_TEXTURE0);
	switch (frameBufferType)
	{
	case POST_PROCESS:
		glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);	// use the color attachment texture as the texture of the quad plane
		break;
	case SHADOW:
		glBindTexture(GL_TEXTURE_2D, depthMap);	// use the color attachment texture as the texture of the quad plane
		//glBindTexture(GL_TEXTURE_2D, depthMap);	// use the color attachment texture as the texture of the quad plane
		break;
	}
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void FrameBuffer::DrawWithShaderAndTexture(Shader* _shader, unsigned int _textureObject)
{
	if (_shader == nullptr) return;
	// now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// disable depth test so screen-space quad isn't discarded due to depth test
	glDisable(GL_DEPTH_TEST);

	// clear all relevant buffers
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	_shader->use();
	_shader->setInt("PostProcessEffect", (int)0);

	glBindVertexArray(quadVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _textureObject);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void FrameBuffer::DoGaussianBlur(unsigned int _textureObject, bool _horizontal)
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	// disable depth test so screen-space quad isn't discarded due to depth test
	glDisable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	GaussianBlurShader->use();
	GaussianBlurShader->setBool("horizontal", _horizontal);
	glBindVertexArray(quadVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _textureObject);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void FrameBuffer::DrawBloomResult(unsigned int _gaussianBlurTexture, unsigned int _normalTexture)
{
	// now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// disable depth test so screen-space quad isn't discarded due to depth test
	glDisable(GL_DEPTH_TEST);

	// clear all relevant buffers
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	bloomShader->use();
	glBindVertexArray(quadVAO);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(bloomShader->ID, "scene"), 0);
	glBindTexture(GL_TEXTURE_2D, _normalTexture);
	glActiveTexture(GL_TEXTURE1);
	glUniform1i(glGetUniformLocation(bloomShader->ID, "bloomBlur"), 1);
	glBindTexture(GL_TEXTURE_2D, _gaussianBlurTexture);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}
