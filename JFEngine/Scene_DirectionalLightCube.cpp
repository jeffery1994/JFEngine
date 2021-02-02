///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#include "Scene_DirectionalLightCube.h"
#include "RenderObject_Quad.h"
#include "RenderObject_Cube.h"
#include "RenderObject_Skybox.h"
#include "camera.h"
#include "ResourceData.def"
#include "FrameBuffer.h"
#include <sstream>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Scene_DirectionalLightCube::Scene_DirectionalLightCube(float _width, float _height, Camera* _camera)
	:Scene(_width, _height, _camera)
{
	Init();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Scene_DirectionalLightCube::Init()
{	
	//Skybox
	//{
	//	RenderObjectFactory_Skybox* factory = new RenderObjectFactory_Skybox();
	//	renderObjects.push_back(unique_ptr<RenderObject>(factory->CreateRenderObject("Skybox.vert", "Skybox.frag")));
	//	delete factory;
	//}

	//Quad - AKA Floor
	{
		RenderObjectFactory_Quad* factory = new RenderObjectFactory_Quad();
		renderObjects.push_back(unique_ptr<RenderObject>(factory->CreateRenderObject("Universal.vert", "Universal.frag", "Outline.vert", "Outline.frag")));
		delete factory;

		//Add floor texture
		renderObjects[renderObjects.size() - 1].get()->AddTexture(RESOURCE::WallDiffuseTexture);
		renderObjects[renderObjects.size() - 1].get()->Translate(glm::vec3(0.0f, -7.0f, 0.0f));

		//Enable light calculation for floor
		renderObjects[renderObjects.size() - 1].get()->GetShader()->use();
		renderObjects[renderObjects.size() - 1].get()->GetShader()->setBool("hasLight", true);
	}
	
	//Cube
	{
		RenderObjectFactory_Cube* factory = new RenderObjectFactory_Cube();
		renderObjects.push_back(unique_ptr<RenderObject>(factory->CreateRenderObject("Universal.vert", "Universal.frag", "Outline.vert", "Outline.frag")));
		delete factory;

		//Enable light calculation
		renderObjects[renderObjects.size() - 1].get()->GetShader()->use();
		renderObjects[renderObjects.size() - 1].get()->GetShader()->setBool("hasLight", true);

		renderObjects[renderObjects.size() - 1].get()->Translate(glm::vec3(0.0f, -6.0f, 0.0f));
		renderObjects[renderObjects.size() - 1].get()->AddTexture(RESOURCE::CubeDiffuseTexture);
		renderObjects[renderObjects.size() - 1].get()->AddTexture(RESOURCE::CubeSpecularTexture);
		//renderObjects[renderObjects.size() - 1].get()->SetDrawOutline(true);
	}

	//Cube
	//{
	//	RenderObjectFactory_Cube* factory = new RenderObjectFactory_Cube();
	//	renderObjects.push_back(unique_ptr<RenderObject>(factory->CreateRenderObject("EnvironmentMapping.vert", "EnvironmentMapping.frag", "Outline.vert", "Outline.frag")));
	//	delete factory;
	//	renderObjects[renderObjects.size() - 1].get()->GetShader()->use();
	//	renderObjects[renderObjects.size() - 1].get()->Translate(glm::vec3(2.0f, -6.0f, 0.0f));
	//	//renderObjects[renderObjects.size() - 1].get()->AddCubeMapTexture(RESOURCE::SkyboxTexture);
	//}

	//Glass - AKA window
	//{
	//	RenderObjectFactory_Quad* factory = new RenderObjectFactory_Quad();
	//	renderObjects.push_back(unique_ptr<RenderObject>(factory->CreateRenderObject("Universal.vert", "Universal.frag", "Outline.vert", "Outline.frag")));
	//	delete factory;
	//	//Move window upwards so we can see through the window
	//	renderObjects[renderObjects.size() - 1].get()->Translate(glm::vec3(0.0f, -4.0f, 0.0f));

	//	//Scale down a liitle bit to make it more window-like
	//	renderObjects[renderObjects.size() - 1].get()->Scale(glm::vec3(0.2f, 0.2f, 0.2f));

	//	//Add glass texture
	//	renderObjects[renderObjects.size() - 1].get()->AddTexture(RESOURCE::GlassDiffuseTexture);

	//	//Disable light for window for now
	//	renderObjects[renderObjects.size() - 1].get()->GetShader()->use();
	//	renderObjects[renderObjects.size() - 1].get()->GetShader()->setBool("hasLight", false);
	//}

	//Config ogl global state
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_MULTISAMPLE); // Enabled by default on some drivers, but not all so always enable to make sure
	//glEnable(GL_CULL_FACE);

	//Create Light
	//Lights.push_back(unique_ptr<Light>(new Light(
	//	glm::vec3(-2.0f, 4.0f, -1.0f),
	//	glm::vec3(1.0f, -1.0f, 1.0f),
	//	glm::vec3(0.1f, 0.1f, 0.1f),
	//	glm::vec3(0.5f, 0.5f, 0.5f),
	//	glm::vec3(1.0f, 1.0f, 1.0f)
	//)));

	Lights.push_back(unique_ptr<Light>(new SpotLight(
		glm::vec3(2.0f, 4.0f, 0.0f),
		glm::vec3(0.1f, 0.1f, 0.1f),
		glm::vec3(0.5f, 0.5f, 0.5f),
		glm::vec3(1.0f, 1.0f, 1.0f)
	)));

	//Lights.push_back(unique_ptr<Light>(new PointLight(
	//	glm::vec3(-2.0f, 4.0f, 0.0f),
	//	glm::vec3(0.0f, 0.1f, 0.0f),
	//	glm::vec3(0.0f, 0.5f, 0.0f),
	//	glm::vec3(0.0f, 1.0f, 0.0f)
	//)));

	//Calculate light view space transformation matrix
	GLfloat near_plane = 0.1f, far_plane =100.0f;
	glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
	glm::mat4 lightView = glm::lookAt(-Lights[0].get()->GetDirection(), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	//glm::mat4 view = camera->GetViewMatrix();
	lightSpaceMatrix = lightProjection * lightView;

	for (auto idx = 0; idx < renderObjects.size(); ++idx)
	{
		renderObjects[idx].get()->SetupShadowShader("Shadow.vert", "Shadow.frag");
		renderObjects[idx].get()->SetupDeferedShader("Defered.vert", "Defered.frag");

		//normal shader 
		SetGeneralShaderConstants(renderObjects[idx].get()->GetShader());

		//defered shader
		SetGeneralShaderConstants(renderObjects[idx].get()->GetDeferedShader());
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Scene_DirectionalLightCube::Render()
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
		renderObjects[idx].get()->GetShader()->use();
		renderObjects[idx].get()->GetShader()->setVec3("viewPos", camera->Position);
		renderObjects[idx].get()->SetProjectionMetrix(projection);
		renderObjects[idx].get()->SetViewMetrix(view);
		renderObjects[idx].get()->PreRender();
		renderObjects[idx].get()->Render();
		renderObjects[idx].get()->PostRender();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Scene_DirectionalLightCube::ShadowPass(FrameBuffer* _shadowBuffer)
{
	if (_shadowBuffer == nullptr) return;
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, _shadowBuffer->GetFrameBuffer());

	//Clear first
	glClearColor(0.1,0.1,0.1,1.0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	for (auto idx = 0; idx < renderObjects.size(); ++idx)
	{
		renderObjects[idx].get()->GetShadowShader()->use();
		renderObjects[idx].get()->GetShadowShader()->setMat4("model", renderObjects[idx].get()->GetModelMetrix());
		renderObjects[idx].get()->GetShadowShader()->setMat4("lightSpaceMatrix", lightSpaceMatrix);
		renderObjects[idx].get()->ShadowPass();
	}
	
	//Render texture to frame buffer
	//_shadowBuffer->SetPostProcessEffect(SHADOW_MAP);
	//_shadowBuffer->Draw();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Scene_DirectionalLightCube::NormalPass(FrameBuffer* _frameBuffer, FrameBuffer* _shadowBuffer)
{
	if (_frameBuffer == nullptr || _shadowBuffer == nullptr ) return;
	glBindFramebuffer(GL_FRAMEBUFFER, _frameBuffer->GetFrameBuffer());

	//Clear first
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	//Render to texture
	glm::mat4 projection = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);
	if (camera != nullptr)
	{
		projection = glm::perspective(glm::radians(camera->Zoom), (float)width / (float)height, 0.1f, 100.0f);
		view = camera->GetViewMatrix();
	}

	for (auto idx = 0; idx < renderObjects.size(); ++idx)
	{
		int numOfTextures = renderObjects[idx].get()->GetNumberOfTexture();

		UpdateShaderConstants(renderObjects[idx].get()->GetShader(), numOfTextures, _shadowBuffer);

		//renderObjects[idx].get()->GetShader()->use();
		////Bind depth texture
		//glActiveTexture(GL_TEXTURE0 + numOfTextures);
		//glUniform1i(glGetUniformLocation(renderObjects[idx].get()->GetShader()->ID, "shadowMap"), numOfTextures);
		//glBindTexture(GL_TEXTURE_2D, _shadowBuffer->GetShadowMap());
		//renderObjects[idx].get()->GetShader()->setMat4("lightSpaceMatrix", lightSpaceMatrix);
		//renderObjects[idx].get()->GetShader()->setVec3("viewPos", camera->Position);
		renderObjects[idx].get()->SetProjectionMetrix(projection);
		renderObjects[idx].get()->SetViewMetrix(view);
		renderObjects[idx].get()->PreRender();
		renderObjects[idx].get()->Render();
		renderObjects[idx].get()->PostRender();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Scene_DirectionalLightCube::PostProcessPass(FrameBuffer* _frameBuffer)
{
	//Render texture to frame buffer
	if (_frameBuffer == nullptr) return;
	_frameBuffer->SetPostProcessEffect(HDR);
	_frameBuffer->Draw();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Scene_DirectionalLightCube::BloomPass(FrameBuffer* _pingPongBuffer1, FrameBuffer* _pingPongBuffer2, unsigned int texture)
{
	if (_pingPongBuffer1 == nullptr || _pingPongBuffer2 == nullptr) return;
	static const unsigned int amount = 10;
	_pingPongBuffer1->DoGaussianBlur(texture, true);
	for (auto idx = 0; idx < amount - 1; ++idx)
	{
		if (idx % 2 == 0)
		{
			_pingPongBuffer2->DoGaussianBlur(_pingPongBuffer1->GetColorBufferByIndex(0), false);
		}
		else
		{
			_pingPongBuffer1->DoGaussianBlur(_pingPongBuffer2->GetColorBufferByIndex(0), false);
		}
	}

	//_pingPongBuffer2->SetPostProcessEffect(NO_EFFECT);
	//_pingPongBuffer2->Draw();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Scene_DirectionalLightCube::DeferedPass(FrameBuffer* _frameBuffer, FrameBuffer* _shadowBuffer)
{
	if (_frameBuffer == nullptr || _shadowBuffer == nullptr) return;
	glBindFramebuffer(GL_FRAMEBUFFER, _frameBuffer->GetFrameBuffer());

	//Clear first
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	//Render to texture
	glm::mat4 projection = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);
	if (camera != nullptr)
	{
		projection = glm::perspective(glm::radians(camera->Zoom), (float)width / (float)height, 0.1f, 100.0f);
		view = camera->GetViewMatrix();
	}

	for (auto idx = 0; idx < renderObjects.size(); ++idx)
	{
		int numOfTextures = renderObjects[idx].get()->GetNumberOfTexture();
		renderObjects[idx].get()->GetDeferedShader()->use();

		//Bind depth texture
		glActiveTexture(GL_TEXTURE0 + numOfTextures);
		glUniform1i(glGetUniformLocation(renderObjects[idx].get()->GetShader()->ID, "shadowMap"), numOfTextures);
		glBindTexture(GL_TEXTURE_2D, _shadowBuffer->GetShadowMap());
		renderObjects[idx].get()->GetDeferedShader()->setMat4("lightSpaceMatrix", lightSpaceMatrix);
		renderObjects[idx].get()->GetDeferedShader()->setVec3("viewPos", camera->Position);
		renderObjects[idx].get()->SetProjectionMetrix(projection);
		renderObjects[idx].get()->SetViewMetrix(view);
		renderObjects[idx].get()->PreRender();
		renderObjects[idx].get()->Render(renderObjects[idx].get()->GetDeferedShader());
		renderObjects[idx].get()->PostRender();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Scene_DirectionalLightCube::PresentToScreen(Shader* _shader, unsigned int _texture)
{
	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Scene_DirectionalLightCube::DeferedFinalPass(Shader* _shader, unsigned int _screenQuadVBO, unsigned int _positionTexture, unsigned int _normalTexture, unsigned int _albedoSpecTexture, FrameBuffer* _frameBuffer)
{
	if (_frameBuffer == nullptr) glBindFramebuffer(GL_FRAMEBUFFER, 0);
	else glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// disable depth test so screen-space quad isn't discarded due to depth test
	glDisable(GL_DEPTH_TEST);

	// clear all relevant buffers
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	_shader->use();
	_shader->setInt("PostProcessEffect", (int)0);

	glBindVertexArray(_screenQuadVBO);

	//Bind g-buffers
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(_shader->ID, "gPosition"), 0);
	glBindTexture(GL_TEXTURE_2D, _positionTexture);
	glActiveTexture(GL_TEXTURE1);
	glUniform1i(glGetUniformLocation(_shader->ID, "gNormal"), 1);
	glBindTexture(GL_TEXTURE_2D, _normalTexture);
	glActiveTexture(GL_TEXTURE2);
	glUniform1i(glGetUniformLocation(_shader->ID, "gAlbedoSpec"), 2);
	glBindTexture(GL_TEXTURE_2D, _albedoSpecTexture);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Scene_DirectionalLightCube::DeferedFinalPass(Shader* _shader, unsigned int _screenQuadVBO, unsigned int _positionTexture, unsigned int _normalTexture, unsigned int _albedoTexture, unsigned int _specTexture, FrameBuffer* _frameBuffer)
{
	if (_frameBuffer == nullptr) glBindFramebuffer(GL_FRAMEBUFFER, 0);
	else glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// disable depth test so screen-space quad isn't discarded due to depth test
	glDisable(GL_DEPTH_TEST);

	// clear all relevant buffers
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	_shader->use();
	_shader->setVec3("viewPos", camera->Position);
	_shader->setVec3("light.ambient", Lights[0].get()->GetAmbient());
	_shader->setVec3("light.diffuse", Lights[0].get()->GetDiffuse()); // darken diffuse light a bit
	_shader->setVec3("light.specular", Lights[0].get()->GetSpecular());
	_shader->setVec3("light.direction", Lights[0].get()->GetDirection());

	glBindVertexArray(_screenQuadVBO);

	//Bind g-buffers
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(_shader->ID, "gPosition"), 0);
	glBindTexture(GL_TEXTURE_2D, _positionTexture);
	glActiveTexture(GL_TEXTURE1);
	glUniform1i(glGetUniformLocation(_shader->ID, "gNormal"), 1);
	glBindTexture(GL_TEXTURE_2D, _normalTexture);
	glActiveTexture(GL_TEXTURE2);
	glUniform1i(glGetUniformLocation(_shader->ID, "gAlbedo"), 2);
	glBindTexture(GL_TEXTURE_2D, _albedoTexture);
	glActiveTexture(GL_TEXTURE3);
	glUniform1i(glGetUniformLocation(_shader->ID, "gSpec"), 3);
	glBindTexture(GL_TEXTURE_2D, _specTexture);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Scene_DirectionalLightCube::SetGeneralShaderConstants(Shader* _shader)
{
	if (_shader == nullptr) return;
	_shader->use();
	_shader->setBool("hasLight", true);
	_shader->setInt("numberOfLights", (int)Lights.size());
	_shader->setVec3("viewPos", camera->Position);
	std::string lightPost = "]";
	for (auto idx = 0; idx < Lights.size(); ++idx)
	{
		stringstream ss;
		ss << "lights[" << idx << "]";
		string lightName = ss.str();
		_shader->setInt(lightName + ".type", Lights[idx].get()->GetLightType());
		_shader->setVec3(lightName + ".position", Lights[idx].get()->GetPosition());
		_shader->setVec3(lightName + ".ambient", Lights[idx].get()->GetAmbient());
		_shader->setVec3(lightName + ".diffuse", Lights[idx].get()->GetDiffuse()); // darken diffuse light a bit
		_shader->setVec3(lightName + ".specular", Lights[idx].get()->GetSpecular());

		if (Lights[idx].get()->GetLightType() == DIRECTIONAL_LIGHT)
		{
			_shader->setVec3(lightName + ".direction", Lights[idx].get()->GetDirection());
		}
		else if (Lights[idx].get()->GetLightType() == POINT_LIGHT)
		{
			PointLight* pointLight = dynamic_cast<PointLight*>(Lights[idx].get());
			if (pointLight == nullptr) continue;
			_shader->setFloat(lightName + ".constant", pointLight->GetConstant());
			_shader->setFloat(lightName + ".linear", pointLight->GetLinear());
			_shader->setFloat(lightName + ".quadratic", pointLight->GetQuadratic());
		}
		else if (Lights[idx].get()->GetLightType() == SPOT_LIGHT)
		{
			SpotLight* spotLight = dynamic_cast<SpotLight*>(Lights[idx].get());
			if (spotLight == nullptr) continue;
			spotLight->SetPosition(camera->Position);
			spotLight->SetDirection(camera->Front);
			_shader->setVec3(lightName + ".position", Lights[idx].get()->GetPosition());
			_shader->setVec3(lightName + ".direction", Lights[idx].get()->GetDirection());
			_shader->setFloat(lightName + ".constant", spotLight->GetConstant());
			_shader->setFloat(lightName + ".linear", spotLight->GetLinear());
			_shader->setFloat(lightName + ".quadratic", spotLight->GetQuadratic());
			_shader->setFloat(lightName + ".cutOffAngleCosine", spotLight->GetCutOffAngleCosine());
		}
	}
	_shader->setFloat("shininess", 64.0f);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Scene_DirectionalLightCube::UpdateShaderConstants(Shader* _shader, int _numOfTextures, FrameBuffer* _shadowBuffer)
{
	if (_shader == nullptr) return;
	_shader->use();
	//Bind depth texture
	glActiveTexture(GL_TEXTURE0 + _numOfTextures);
	glUniform1i(glGetUniformLocation(_shader->ID, "shadowMap"), _numOfTextures);
	if (_shadowBuffer != nullptr)
	{
		glBindTexture(GL_TEXTURE_2D, _shadowBuffer->GetShadowMap());
	}
	_shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
	_shader->setVec3("viewPos", camera->Position);
	for (auto idx = 0; idx < Lights.size(); ++idx)
	{
		stringstream ss;
		ss << "lights[" << idx << "]";
		string lightName = ss.str();
		if (Lights[idx].get()->GetLightType() == SPOT_LIGHT)
		{
			SpotLight* spotLight = dynamic_cast<SpotLight*>(Lights[idx].get());
			if (spotLight == nullptr) continue;
			spotLight->SetPosition(camera->Position);
			spotLight->SetDirection(camera->Front);
			_shader->setVec3(lightName + ".position", Lights[idx].get()->GetPosition());
			_shader->setVec3(lightName + ".direction", Lights[idx].get()->GetDirection());
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Scene* SceneFactory_DirectionalLightCube::CreateScene(float _width, float _height, Camera* _camera)
{
	return new Scene_DirectionalLightCube(_width, _height, _camera);
}