#include "jfrendersystem.h"
#include <iostream>
#include "RenderNode.h"
#include "JFOpenGL.h"
#include "camera.h"
#include "Cube.h"
#include "RenderObject_Cube.h"
#include "Scene_BasicCube.h"
#include "Scene_BasicModel.h"
#include "Scene_DirectionalLightCube.h"
#include "ShaderManager.h"
#include "TextureManager.h"
#include "Texture.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
JFRENDER_SYSTEM::JFRENDER_SYSTEM()
	:Inited(false)
	, Width(800)
	, Height(600)
	, CurrentEmptyNodeIndex(0)
	, CurrentEmptyObjectIndex(0)
{
	//DeinitRenderNodes();
	//DeinitRenderObjects();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
JFRENDER_SYSTEM::~JFRENDER_SYSTEM()
{
	//DeinitRenderNodes();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void JFRENDER_SYSTEM::InitDefaultCamera()
{
	camera = new Camera(glm::vec3(0.0f, 1.0f, 0.0f));
}

void JFRENDER_SYSTEM::InitScreenQuad()
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
}

void JFRENDER_SYSTEM::InitShaderManager()
{
	shaderManager = new ShaderManager();
	shaderManager->AddShaderByType(new Shader("DeferedFinal.vert", "DeferedFinal.frag"), SHADER_TYPE::SHADER_DEFERED_FINAL);
	shaderManager->AddShaderByType(new Shader("DirectPBR.vert", "DirectPBR.frag"), SHADER_TYPE::SHADER_DIRECT_PBR);
}

void JFRENDER_SYSTEM::InitTextureManager()
{
	textureManager = new TextureManager();
	textureManager->AddTexture("", );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void JFRENDER_SYSTEM::OnFrameBufferSizeChanged(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool JFRENDER_SYSTEM::InitGLFW()
{
	//Init GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	//Init GLFW window
	Window = glfwCreateWindow(Width, Height, "JFEngine", NULL, NULL);
	if (Window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(Window);
	glfwSetFramebufferSizeCallback(Window, OnFrameBufferSizeChanged);

	InitDefaultCamera();

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool JFRENDER_SYSTEM::InitGLAD()
{
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return false;
	}

	glViewport(0, 0, Width, Height);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool JFRENDER_SYSTEM::Init()
{
	if (Inited) return false;
	if (!InitGLFW()) return false;
	if (!InitGLAD()) return false;
	InitFrameBuffers();
	InitScreenQuad();
	InitShaderManager();
	//int nrAttributes;
	//glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
	//std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;
	//SetupCubeScene();
	//SetupCubeLightScene();
	//SetupCubeLightSceneWithMaterial();
	//SetupCubeLightSceneWithLightMaps();
	//SetupCubeLightSceneWithDirectionalLight();
	//SetupCubeLightSceneWithPointLight();
	//SetupCubeLightSceneWithSpotLight();
	//SetupCubeLightSceneWithMultipleLights();
	//InitModels();
	//SetupModelScene();
	//SetupRenderObjectCubeScene();
	CreateScene();
	Inited = true;
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void JFRENDER_SYSTEM::DeinitGLFW()
{
	glfwTerminate();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void JFRENDER_SYSTEM::Deinit()
{
	if (!Inited) return;
	DeinitRenderNodes();
	DeinitRenderObjects();
	if (CurrentScene != nullptr)
	{
		CurrentScene->Deinit();
	}
	DeinitGLFW();
	Inited = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void JFRENDER_SYSTEM::Update(float DeltaTime)
{
	if (!Inited) return;

	//Clear buffer
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	//Render all nodes in current scene
	RenderRenderNodes();
	RenderRenderObjects();
	if (CurrentScene != nullptr)
	{
		FrameBuffer* postProcessBuffer = GetFrameBufferByType(FRAME_BUFFER_TYPE::POST_PROCESS);
		FrameBuffer* shadowBuffer = GetFrameBufferByType(FRAME_BUFFER_TYPE::SHADOW);
		FrameBuffer* pingPongBuffer1 = GetFrameBufferByType(FRAME_BUFFER_TYPE::PING_PONG_ONE);
		FrameBuffer* pingPongBuffer2 = GetFrameBufferByType(FRAME_BUFFER_TYPE::PING_PONG_TWO);

		//Shadow pass
		if (renderShadows && shadowBuffer != nullptr)
		{
			CurrentScene->ShadowPass(shadowBuffer);
		}

		//Defered pass
		if (usingDeferedShading)
		{
			if (postProcessBuffer != nullptr)
			{
				CurrentScene->DeferedPass(postProcessBuffer, shadowBuffer);
			}
		}
		//Direct PBR Pass
		else if (usingDirectPBR && postProcessBuffer != nullptr)
		{
			Shader* directPBRShader = shaderManager->GetShaderByType(SHADER_TYPE::SHADER_DIRECT_PBR);
			CurrentScene->DirectPBRPass(postProcessBuffer, directPBRShader);
		}
		//normal pass
		else
		{
			if (postProcessBuffer != nullptr)
			{
				CurrentScene->NormalPass(postProcessBuffer, shadowBuffer);
				//CurrentScene->PostProcessPass(postProcessBuffer);
			}

		}

		//bloom pass
		if (usingBloom)
		{
			if (pingPongBuffer1 != nullptr && pingPongBuffer2 != nullptr)
			{
				CurrentScene->BloomPass(pingPongBuffer1, pingPongBuffer2, postProcessBuffer->GetColorBufferByIndex(1));
				pingPongBuffer1->DrawBloomResult(pingPongBuffer2->GetColorBufferByIndex(0), postProcessBuffer->GetColorBufferByIndex(0));
			}
		}
		else if (usingDeferedShading)
		{
			if (postProcessBuffer != nullptr)
			{
				Shader* deferedShader = shaderManager->GetShaderByType(SHADER_TYPE::SHADER_DEFERED_FINAL);
				if (deferedShader == nullptr) return;
				CurrentScene->DeferedFinalPass(
					deferedShader,
					quadVAO,
					postProcessBuffer->GetColorBufferByIndex(0),
					postProcessBuffer->GetColorBufferByIndex(1),
					postProcessBuffer->GetColorBufferByIndex(2),
					postProcessBuffer->GetColorBufferByIndex(3)
				);
				//CurrentScene->PresentToScreen(postProcessBuffer, postProcessBuffer->GetProcessShader(), postProcessBuffer->GetColorBufferByIndex(2));
			}
		}
		//post process pass except for bloom
		else
		{
			if (postProcessBuffer != nullptr)
			{
				CurrentScene->PostProcessPass(postProcessBuffer);
			}
		}
	}
	
	//swap buffers
	glfwSwapBuffers(Window);
	glfwPollEvents();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool JFRENDER_SYSTEM::ShouldExit()
{
	return glfwWindowShouldClose(Window);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void JFRENDER_SYSTEM::InitRenderNodes()
{
	for (auto idx = 0; idx < MAX_NUM_OF_RENDERNODE_PER_SCENE; ++idx)
	{
		if (Nodes[idx] == nullptr) break;
		Nodes[idx]->Setup();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void JFRENDER_SYSTEM::DeinitRenderNodes()
{
	for (auto idx = 0; idx < MAX_NUM_OF_RENDERNODE_PER_SCENE; ++idx)
	{
		if (Nodes[idx] == nullptr) continue;
		Nodes[idx]->Destroy();
		delete Nodes[idx];
		Nodes[idx] = nullptr;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void JFRENDER_SYSTEM::RenderRenderNodes()
{
	//camera projection matrix & camera view matrix
	glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)Width / (float)Height, 0.1f, 100.0f);
	glm::mat4 view = camera->GetViewMatrix();

	for (auto idx = 0; idx < MAX_NUM_OF_RENDERNODE_PER_SCENE; ++idx)
	{
		if (Nodes[idx] == nullptr) break;
		Nodes[idx]->SetProjectionMetrix(projection);
		Nodes[idx]->SetViewMetrix(view);
		Nodes[idx]->PreRender();
		Nodes[idx]->GetShader()->setVec3("viewPos", camera->Position);
		Nodes[idx]->GetShader()->setVec3("light.position", camera->Position);
		Nodes[idx]->GetShader()->setVec3("light.direction", camera->Front);
		Nodes[idx]->GetShader()->setVec3("spotLight.position", camera->Position);
		Nodes[idx]->GetShader()->setVec3("spotLight.direction", camera->Front);
		Nodes[idx]->Render();
		Nodes[idx]->PostRender();
	}
}

///////////////////////////////////////////////////////////////////////////////
// TODO: do a scene class to manage render nodes
///////////////////////////////////////////////////////////////////////////////
void JFRENDER_SYSTEM::SetupCubeScene()
{
	//Cube 1
	Nodes[CurrentEmptyNodeIndex] = new Cube();
	if (!Nodes[CurrentEmptyNodeIndex]->SetupShaders("4.2.texture.vert", "4.2.texture.frag"))
	{
		return;
	}
	if (!Nodes[CurrentEmptyNodeIndex]->SetupTextureByIndex("../textures/container.jpg", 0))
	{
		return;
	}
	if (!Nodes[CurrentEmptyNodeIndex]->SetupTextureByIndex("../textures/awesomeface.png", 1, true))
	{
		return;
	}
	Nodes[CurrentEmptyNodeIndex]->Setup();
	Nodes[CurrentEmptyNodeIndex]->SetPosition(glm::vec3(0.0, 0.0, -10.0));
	//Nodes[CurrentEmptyNodeIndex]->Translate(glm::vec3(0.0, 0.0, -10.0));
	CurrentEmptyNodeIndex++;

	//Cube 2
	Nodes[CurrentEmptyNodeIndex] = new Cube();
	if (!Nodes[CurrentEmptyNodeIndex]->SetupShaders("4.2.texture.vert", "4.2.texture.frag"))
	{
		return;
	}
	if (!Nodes[CurrentEmptyNodeIndex]->SetupTextureByIndex("../textures/container.jpg", 0))
	{
		return;
	}
	if (!Nodes[CurrentEmptyNodeIndex]->SetupTextureByIndex("../textures/awesomeface.png", 1, true))
	{
		return;
	}
	Nodes[CurrentEmptyNodeIndex]->Setup();
	Nodes[CurrentEmptyNodeIndex]->SetPosition(glm::vec3(1.0, 0.0, 0.0));
	//Nodes[CurrentEmptyNodeIndex]->Translate(glm::vec3(1.0, 0.0, 0.0));
	CurrentEmptyNodeIndex++;
}

///////////////////////////////////////////////////////////////////////////////
// TODO: do a scene class to manage render nodes
///////////////////////////////////////////////////////////////////////////////
void JFRENDER_SYSTEM::SetupCubeLightScene()
{
	//light cube
	Nodes[CurrentEmptyNodeIndex] = new Cube();
	if (!Nodes[CurrentEmptyNodeIndex]->SetupShaders("SimpleCube.vert", "SimpleCube.frag")) return;
	Nodes[CurrentEmptyNodeIndex]->Setup();
	Nodes[CurrentEmptyNodeIndex]->SetPosition(glm::vec3(0.0, 3.0, 0.0));
	Nodes[CurrentEmptyNodeIndex]->SetColor(glm::vec4(1.0, 1.0, 1.0, 1.0));
	Nodes[CurrentEmptyNodeIndex]->Scale(glm::vec3(0.2));
	glm::vec3 LightPos = Nodes[CurrentEmptyNodeIndex]->GetPosition();
	CurrentEmptyNodeIndex++;
	
	//casted cube
	Nodes[CurrentEmptyNodeIndex] = new Cube();
	if (!Nodes[CurrentEmptyNodeIndex]->SetupShaders("LightingCube.vert", "LightingCube.frag")) return;
	Nodes[CurrentEmptyNodeIndex]->Setup();
	Nodes[CurrentEmptyNodeIndex]->SetPosition(glm::vec3(0, 0, 0));
	Nodes[CurrentEmptyNodeIndex]->GetShader()->use();
	Nodes[CurrentEmptyNodeIndex]->GetShader()->setVec3("objectColor", glm::vec3(1.0, 0.5, 0.31f));
	Nodes[CurrentEmptyNodeIndex]->GetShader()->setVec3("lightColor", glm::vec3(1.0, 1.0, 1.0));
	Nodes[CurrentEmptyNodeIndex]->GetShader()->setVec3("lightPos", LightPos);
	Nodes[CurrentEmptyNodeIndex]->GetShader()->setVec3("viewPos", camera->Position);
	CurrentEmptyNodeIndex++;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void JFRENDER_SYSTEM::SetupCubeLightSceneWithMaterial()
{
	//light cube
	Nodes[CurrentEmptyNodeIndex] = new Cube();
	if (!Nodes[CurrentEmptyNodeIndex]->SetupShaders("SimpleCube.vert", "SimpleCube.frag")) return;
	Nodes[CurrentEmptyNodeIndex]->Setup();
	Nodes[CurrentEmptyNodeIndex]->SetPosition(glm::vec3(0.0, 3.0, 0.0));
	Nodes[CurrentEmptyNodeIndex]->SetColor(glm::vec4(1.0, 1.0, 1.0, 1.0));
	Nodes[CurrentEmptyNodeIndex]->Scale(glm::vec3(0.2));
	glm::vec3 LightPos = Nodes[CurrentEmptyNodeIndex]->GetPosition();
	CurrentEmptyNodeIndex++;

	//casted cube
	Nodes[CurrentEmptyNodeIndex] = new Cube();
	if (!Nodes[CurrentEmptyNodeIndex]->SetupShaders("LightingMaterialCube.vert", "LightingMaterialCube.frag")) return;
	Nodes[CurrentEmptyNodeIndex]->Setup();
	Nodes[CurrentEmptyNodeIndex]->SetPosition(glm::vec3(0, 0, 0));
	Nodes[CurrentEmptyNodeIndex]->GetShader()->use();
	Nodes[CurrentEmptyNodeIndex]->GetShader()->setVec3("lightPos", LightPos);
	Nodes[CurrentEmptyNodeIndex]->GetShader()->setVec3("viewPos", camera->Position);
	Nodes[CurrentEmptyNodeIndex]->GetShader()->setVec3("material.ambient", glm::vec3(1.0f, 0.5f, 0.31f));
	Nodes[CurrentEmptyNodeIndex]->GetShader()->setVec3("material.diffuse", glm::vec3(1.0f, 0.5f, 0.31f));
	Nodes[CurrentEmptyNodeIndex]->GetShader()->setVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
	Nodes[CurrentEmptyNodeIndex]->GetShader()->setFloat("material.shininess", 64.0f);
	Nodes[CurrentEmptyNodeIndex]->GetShader()->setVec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
	Nodes[CurrentEmptyNodeIndex]->GetShader()->setVec3("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f)); // darken diffuse light a bit
	Nodes[CurrentEmptyNodeIndex]->GetShader()->setVec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
	CurrentEmptyNodeIndex++;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void JFRENDER_SYSTEM::SetupCubeLightSceneWithLightMaps()
{
	//light cube
	Nodes[CurrentEmptyNodeIndex] = new Cube();
	if (!Nodes[CurrentEmptyNodeIndex]->SetupShaders("SimpleCube.vert", "SimpleCube.frag")) return;
	Nodes[CurrentEmptyNodeIndex]->Setup();
	Nodes[CurrentEmptyNodeIndex]->SetPosition(glm::vec3(0.0, 3.0, 0.0));
	Nodes[CurrentEmptyNodeIndex]->SetColor(glm::vec4(1.0, 1.0, 1.0, 1.0));
	Nodes[CurrentEmptyNodeIndex]->Scale(glm::vec3(0.2));
	glm::vec3 LightPos = Nodes[CurrentEmptyNodeIndex]->GetPosition();
	CurrentEmptyNodeIndex++;

	//casted cube
	Nodes[CurrentEmptyNodeIndex] = new Cube();
	if (!Nodes[CurrentEmptyNodeIndex]->SetupShaders("LightmapCube.vert", "LightmapCube.frag")) return;
	if (!Nodes[CurrentEmptyNodeIndex]->SetupTextureByIndex("../textures/container2.png", 0))
	{
		return;
	}
	if (!Nodes[CurrentEmptyNodeIndex]->SetupTextureByIndex("../textures/container2_specular.png", 1))
	{
		return;
	}
	Nodes[CurrentEmptyNodeIndex]->Setup();
	Nodes[CurrentEmptyNodeIndex]->SetPosition(glm::vec3(0, 0, 0));
	Nodes[CurrentEmptyNodeIndex]->GetShader()->use();
	Nodes[CurrentEmptyNodeIndex]->GetShader()->setVec3("lightPos", LightPos);
	Nodes[CurrentEmptyNodeIndex]->GetShader()->setVec3("viewPos", camera->Position);
	Nodes[CurrentEmptyNodeIndex]->GetShader()->setVec3("material.ambient", glm::vec3(1.0f, 0.5f, 0.31f));
	Nodes[CurrentEmptyNodeIndex]->GetShader()->setVec3("material.diffuse", glm::vec3(1.0f, 0.5f, 0.31f));
	Nodes[CurrentEmptyNodeIndex]->GetShader()->setVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
	Nodes[CurrentEmptyNodeIndex]->GetShader()->setFloat("material.shininess", 64.0f);
	Nodes[CurrentEmptyNodeIndex]->GetShader()->setVec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
	Nodes[CurrentEmptyNodeIndex]->GetShader()->setVec3("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f)); // darken diffuse light a bit
	Nodes[CurrentEmptyNodeIndex]->GetShader()->setVec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
	Nodes[CurrentEmptyNodeIndex]->GetShader()->setInt("material.diffuse", 0);
	Nodes[CurrentEmptyNodeIndex]->GetShader()->setInt("material.specular", 1);
	CurrentEmptyNodeIndex++;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void JFRENDER_SYSTEM::QuickSetupLightingCubeRenderNode()
{
	//casted cube
	Nodes[CurrentEmptyNodeIndex] = new Cube();
	if (!Nodes[CurrentEmptyNodeIndex]->SetupShaders("DirectionalLightCube.vert", "DirectionalLightCube.frag")) return;
	if (!Nodes[CurrentEmptyNodeIndex]->SetupTextureByIndex("../textures/container2.png", 0))
	{
		return;
	}
	if (!Nodes[CurrentEmptyNodeIndex]->SetupTextureByIndex("../textures/container2_specular.png", 1))
	{
		return;
	}
	Nodes[CurrentEmptyNodeIndex]->Setup();
	Nodes[CurrentEmptyNodeIndex]->GetShader()->use();
	//Nodes[CurrentEmptyNodeIndex]->GetShader()->setVec3("lightPos", LightPos);
	Nodes[CurrentEmptyNodeIndex]->GetShader()->setVec3("viewPos", camera->Position);
	Nodes[CurrentEmptyNodeIndex]->GetShader()->setVec3("material.ambient", glm::vec3(1.0f, 0.5f, 0.31f));
	Nodes[CurrentEmptyNodeIndex]->GetShader()->setVec3("material.diffuse", glm::vec3(1.0f, 0.5f, 0.31f));
	Nodes[CurrentEmptyNodeIndex]->GetShader()->setVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
	Nodes[CurrentEmptyNodeIndex]->GetShader()->setFloat("material.shininess", 128.0f);
	Nodes[CurrentEmptyNodeIndex]->GetShader()->setVec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
	Nodes[CurrentEmptyNodeIndex]->GetShader()->setVec3("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f)); // darken diffuse light a bit
	Nodes[CurrentEmptyNodeIndex]->GetShader()->setVec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
	Nodes[CurrentEmptyNodeIndex]->GetShader()->setInt("material.diffuse", 0);
	Nodes[CurrentEmptyNodeIndex]->GetShader()->setInt("material.specular", 1);
	//Nodes[CurrentEmptyNodeIndex]->GetShader()->setVec3("light.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
	Nodes[CurrentEmptyNodeIndex]->GetShader()->setVec3("light.direction", glm::vec3(0.0f, -1.0f, 1.0f));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void JFRENDER_SYSTEM::SetupCubeLightSceneWithDirectionalLight()
{
	static unsigned int NumOfCubes = 5;

	//light caster cube
	Nodes[CurrentEmptyNodeIndex] = new Cube();
	if (!Nodes[CurrentEmptyNodeIndex]->SetupShaders("SimpleCube.vert", "SimpleCube.frag")) return;
	Nodes[CurrentEmptyNodeIndex]->Setup();
	Nodes[CurrentEmptyNodeIndex]->SetPosition(glm::vec3(0.0, 3.0, -3.0));
	Nodes[CurrentEmptyNodeIndex]->SetColor(glm::vec4(1.0, 1.0, 1.0, 1.0));
	Nodes[CurrentEmptyNodeIndex]->Scale(glm::vec3(0.2));
	glm::vec3 LightPos = Nodes[CurrentEmptyNodeIndex]->GetPosition();
	CurrentEmptyNodeIndex++;

	//LightingCube
	for (auto idx = 0; idx < (int)NumOfCubes; ++idx)
	{
		QuickSetupLightingCubeRenderNode();
		Nodes[CurrentEmptyNodeIndex]->SetPosition(glm::vec3(-4 + idx * 2, 0, 0));
		CurrentEmptyNodeIndex++;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void JFRENDER_SYSTEM::SetupCubeLightSceneWithPointLight()
{
	static unsigned int NumOfCubes = 5;

	//light caster cube
	Nodes[CurrentEmptyNodeIndex] = new Cube();
	if (!Nodes[CurrentEmptyNodeIndex]->SetupShaders("SimpleCube.vert", "SimpleCube.frag")) return;
	Nodes[CurrentEmptyNodeIndex]->Setup();
	Nodes[CurrentEmptyNodeIndex]->SetPosition(glm::vec3(0.0, 2.0, -2.0));
	Nodes[CurrentEmptyNodeIndex]->SetColor(glm::vec4(1.0, 1.0, 1.0, 1.0));
	Nodes[CurrentEmptyNodeIndex]->Scale(glm::vec3(0.2));
	glm::vec3 LightPos = Nodes[CurrentEmptyNodeIndex]->GetPosition();
	CurrentEmptyNodeIndex++;

	//LightingCube
	for (auto idx = 0; idx < (int)NumOfCubes; ++idx)
	{
		Nodes[CurrentEmptyNodeIndex] = new Cube();
		if (!Nodes[CurrentEmptyNodeIndex]->SetupShaders("PointLightCube.vert", "PointLightCube.frag")) return;
		if (!Nodes[CurrentEmptyNodeIndex]->SetupTextureByIndex("../textures/container2.png", 0))
		{
			return;
		}
		if (!Nodes[CurrentEmptyNodeIndex]->SetupTextureByIndex("../textures/container2_specular.png", 1))
		{
			return;
		}
		Nodes[CurrentEmptyNodeIndex]->Setup();
		Nodes[CurrentEmptyNodeIndex]->GetShader()->use();
		Nodes[CurrentEmptyNodeIndex]->GetShader()->setVec3("viewPos", camera->Position);
		Nodes[CurrentEmptyNodeIndex]->GetShader()->setInt("material.diffuse", 0);
		Nodes[CurrentEmptyNodeIndex]->GetShader()->setInt("material.specular", 1);
		Nodes[CurrentEmptyNodeIndex]->GetShader()->setFloat("material.shininess", 64.0f);
		Nodes[CurrentEmptyNodeIndex]->GetShader()->setVec3("light.position", LightPos);
		Nodes[CurrentEmptyNodeIndex]->GetShader()->setVec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
		Nodes[CurrentEmptyNodeIndex]->GetShader()->setVec3("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f)); // darken diffuse light a bit
		Nodes[CurrentEmptyNodeIndex]->GetShader()->setVec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
		Nodes[CurrentEmptyNodeIndex]->GetShader()->setFloat("light.constant", 1.0f);
		Nodes[CurrentEmptyNodeIndex]->GetShader()->setFloat("light.linear", 0.09f);
		Nodes[CurrentEmptyNodeIndex]->GetShader()->setFloat("light.quadratic", 0.032f);
		Nodes[CurrentEmptyNodeIndex]->SetPosition(glm::vec3(-4 + idx * 2, 0, 0));
		CurrentEmptyNodeIndex++;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void JFRENDER_SYSTEM::SetupCubeLightSceneWithSpotLight()
{
	static unsigned int NumOfCubes = 5;

	//light caster cube
	Nodes[CurrentEmptyNodeIndex] = new Cube();
	if (!Nodes[CurrentEmptyNodeIndex]->SetupShaders("SimpleCube.vert", "SimpleCube.frag")) return;
	Nodes[CurrentEmptyNodeIndex]->Setup();
	Nodes[CurrentEmptyNodeIndex]->SetPosition(glm::vec3(0.0, 2.0, -2.0));
	Nodes[CurrentEmptyNodeIndex]->SetColor(glm::vec4(1.0, 1.0, 1.0, 1.0));
	Nodes[CurrentEmptyNodeIndex]->Scale(glm::vec3(0.2));
	glm::vec3 LightPos = Nodes[CurrentEmptyNodeIndex]->GetPosition();
	CurrentEmptyNodeIndex++;

	//LightingCube
	for (auto idx = 0; idx < (int)NumOfCubes; ++idx)
	{
		Nodes[CurrentEmptyNodeIndex] = new Cube();
		if (!Nodes[CurrentEmptyNodeIndex]->SetupShaders("SpotLightCube.vert", "SpotLightCubeSoftEdge.frag")) return;
		if (!Nodes[CurrentEmptyNodeIndex]->SetupTextureByIndex("../textures/container2.png", 0))
		{
			return;
		}
		if (!Nodes[CurrentEmptyNodeIndex]->SetupTextureByIndex("../textures/container2_specular.png", 1))
		{
			return;
		}
		Nodes[CurrentEmptyNodeIndex]->Setup();
		Nodes[CurrentEmptyNodeIndex]->GetShader()->use();
		Nodes[CurrentEmptyNodeIndex]->GetShader()->setVec3("viewPos", camera->Position);
		Nodes[CurrentEmptyNodeIndex]->GetShader()->setInt("material.diffuse", 0);
		Nodes[CurrentEmptyNodeIndex]->GetShader()->setInt("material.specular", 1);
		Nodes[CurrentEmptyNodeIndex]->GetShader()->setFloat("material.shininess", 64.0f);
		Nodes[CurrentEmptyNodeIndex]->GetShader()->setVec3("light.position", camera->Position);
		Nodes[CurrentEmptyNodeIndex]->GetShader()->setVec3("light.direction", camera->Front);
		Nodes[CurrentEmptyNodeIndex]->GetShader()->setFloat("light.cutOff", glm::cos(glm::radians(12.5f)));
		Nodes[CurrentEmptyNodeIndex]->GetShader()->setFloat("light.outerCutOff", glm::cos(glm::radians(17.5f)));
		Nodes[CurrentEmptyNodeIndex]->GetShader()->setVec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
		Nodes[CurrentEmptyNodeIndex]->GetShader()->setVec3("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f)); // darken diffuse light a bit
		Nodes[CurrentEmptyNodeIndex]->GetShader()->setVec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
		Nodes[CurrentEmptyNodeIndex]->GetShader()->setFloat("light.constant", 1.0f);
		Nodes[CurrentEmptyNodeIndex]->GetShader()->setFloat("light.linear", 0.09f);
		Nodes[CurrentEmptyNodeIndex]->GetShader()->setFloat("light.quadratic", 0.032f);
		Nodes[CurrentEmptyNodeIndex]->SetPosition(glm::vec3(-4 + idx * 2, 0, 0));
		CurrentEmptyNodeIndex++;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void JFRENDER_SYSTEM::SetupCubeLightSceneWithMultipleLights()
{
	static unsigned int NumOfCubes = 5;

	//light caster cube
	Nodes[CurrentEmptyNodeIndex] = new Cube();
	if (!Nodes[CurrentEmptyNodeIndex]->SetupShaders("SimpleCube.vert", "SimpleCube.frag")) return;
	Nodes[CurrentEmptyNodeIndex]->Setup();
	Nodes[CurrentEmptyNodeIndex]->SetPosition(glm::vec3(0.0, 2.0, -2.0));
	Nodes[CurrentEmptyNodeIndex]->SetColor(glm::vec4(1.0, 1.0, 1.0, 1.0));
	Nodes[CurrentEmptyNodeIndex]->Scale(glm::vec3(0.2));
	glm::vec3 LightPos = Nodes[CurrentEmptyNodeIndex]->GetPosition();
	CurrentEmptyNodeIndex++;

	//LightingCube
	for (auto idx = 0; idx < (int)NumOfCubes; ++idx)
	{
		Nodes[CurrentEmptyNodeIndex] = new Cube();
		if (!Nodes[CurrentEmptyNodeIndex]->SetupShaders("MultipleLights.vert", "MultipleLights.frag")) return;
		if (!Nodes[CurrentEmptyNodeIndex]->SetupTextureByIndex("../textures/container2.png", 0))
		{
			return;
		}
		if (!Nodes[CurrentEmptyNodeIndex]->SetupTextureByIndex("../textures/container2_specular.png", 1))
		{
			return;
		}
		Nodes[CurrentEmptyNodeIndex]->Setup();
		Nodes[CurrentEmptyNodeIndex]->GetShader()->use();
		Nodes[CurrentEmptyNodeIndex]->GetShader()->setVec3("viewPos", camera->Position);

		//Material
		Nodes[CurrentEmptyNodeIndex]->GetShader()->setInt("material.diffuse", 0);
		Nodes[CurrentEmptyNodeIndex]->GetShader()->setInt("material.specular", 1);
		Nodes[CurrentEmptyNodeIndex]->GetShader()->setFloat("material.shininess", 64.0f);

		//Directional Light
		Nodes[CurrentEmptyNodeIndex]->GetShader()->setVec3("dirLight.direction", glm::vec3(0.0f, -1.0f, -1.0f));
		Nodes[CurrentEmptyNodeIndex]->GetShader()->setVec3("dirLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
		Nodes[CurrentEmptyNodeIndex]->GetShader()->setVec3("dirLight.diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
		Nodes[CurrentEmptyNodeIndex]->GetShader()->setVec3("dirLight.specular", glm::vec3(0.5f, 0.5f, 0.5f));

		//Point light 1
		Nodes[CurrentEmptyNodeIndex]->GetShader()->setVec3("pointLights[0].position", LightPos);
		Nodes[CurrentEmptyNodeIndex]->GetShader()->setVec3("pointLights[0].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
		Nodes[CurrentEmptyNodeIndex]->GetShader()->setVec3("pointLights[0].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
		Nodes[CurrentEmptyNodeIndex]->GetShader()->setVec3("pointLights[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));
		Nodes[CurrentEmptyNodeIndex]->GetShader()->setFloat("pointLights[0].constant", 1.0f);
		Nodes[CurrentEmptyNodeIndex]->GetShader()->setFloat("pointLights[0].linear", 0.09);
		Nodes[CurrentEmptyNodeIndex]->GetShader()->setFloat("pointLights[0].quadratic", 0.032);

		//Spot light
		Nodes[CurrentEmptyNodeIndex]->GetShader()->setVec3("spotLight.position", camera->Position);
		Nodes[CurrentEmptyNodeIndex]->GetShader()->setVec3("spotLight.direction", camera->Front);
		Nodes[CurrentEmptyNodeIndex]->GetShader()->setVec3("spotLight.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		Nodes[CurrentEmptyNodeIndex]->GetShader()->setVec3("spotLight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
		Nodes[CurrentEmptyNodeIndex]->GetShader()->setVec3("spotLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
		Nodes[CurrentEmptyNodeIndex]->GetShader()->setFloat("spotLight.constant", 1.0f);
		Nodes[CurrentEmptyNodeIndex]->GetShader()->setFloat("spotLight.linear", 0.09);
		Nodes[CurrentEmptyNodeIndex]->GetShader()->setFloat("spotLight.quadratic", 0.032);
		Nodes[CurrentEmptyNodeIndex]->GetShader()->setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
		Nodes[CurrentEmptyNodeIndex]->GetShader()->setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

		Nodes[CurrentEmptyNodeIndex]->SetPosition(glm::vec3(-4 + idx * 2, 0, 0));
		CurrentEmptyNodeIndex++;
	}
}

///////////////////////////////////////////////////////////////////////////////
//
//	Model related functions
//
///////////////////////////////////////////////////////////////////////////////
void JFRENDER_SYSTEM::DeinitRenderObjects()
{
	for (auto idx = 0; idx < renderObjects.size(); ++idx)
	{
		renderObjects[idx].get()->Destroy();
	}
	CurrentEmptyObjectIndex = 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void JFRENDER_SYSTEM::InitRenderObjects()
{
	//cant do this now, since we do not have a scene object and editor to edit scene.
}

void JFRENDER_SYSTEM::InitFrameBuffers()
{
	//Generate multi-render target frame buffer
	FrameBuffers.push_back(unique_ptr<FrameBuffer>(new FrameBuffer((float)Width, (float)Height, 4)));
	//FrameBuffers.push_back(unique_ptr<FrameBuffer>(new FrameBuffer((float)Width, (float)Height, FRAME_BUFFER_TYPE::POST_PROCESS)));
	FrameBuffers.push_back(unique_ptr<FrameBuffer>(new FrameBuffer((float)Width, (float)Height, FRAME_BUFFER_TYPE::SHADOW)));
	FrameBuffers.push_back(unique_ptr<FrameBuffer>(new FrameBuffer((float)Width, (float)Height, FRAME_BUFFER_TYPE::PING_PONG_ONE)));
	FrameBuffers.push_back(unique_ptr<FrameBuffer>(new FrameBuffer((float)Width, (float)Height, FRAME_BUFFER_TYPE::PING_PONG_TWO)));
}

void JFRENDER_SYSTEM::DeinitFrameBuffers()
{
	for (auto idx = 0; idx < FrameBuffers.size(); ++idx)
	{
		if (FrameBuffers[idx] == nullptr) continue;
		FrameBuffers[idx].get()->Deinit();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void JFRENDER_SYSTEM::RenderRenderObjects()
{
	//camera projection matrix & camera view matrix
	glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)Width / (float)Height, 0.1f, 100.0f);
	glm::mat4 view = camera->GetViewMatrix();
	for (auto idx = 0; idx < renderObjects.size(); ++idx)
	{
		renderObjects[idx].get()->SetProjectionMetrix(projection);
		renderObjects[idx].get()->SetViewMetrix(view);
		renderObjects[idx].get()->PreRender();
		renderObjects[idx].get()->Render();
		renderObjects[idx].get()->PostRender();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void JFRENDER_SYSTEM::SetupModelScene()
{
	//Create render object factory
	RenderObjectFactory_Model* factory = new RenderObjectFactory_Model();

	renderObjects.push_back(unique_ptr<RenderObject>(factory->CreateRenderObject()));
	renderObjects[CurrentEmptyObjectIndex].get()->SetupRenderObject("../resource/backpack.obj");
	if(!renderObjects[CurrentEmptyObjectIndex].get()->SetupShader("ModelLoading.vert", "ModelLoading.frag")) return;
	//RenderObject* object = renderObjects[CurrentEmptyObjectIndex].get();
	//if (object == nullptr) return;
	//RenderObject_Model* modelObject = (RenderObject_Model*)(object);
	//if (modelObject == nullptr) return;
	//modelObject->SetupModel("../resource/backpack.obj");
	//if (modelObject->SetupShader("ModelLoading.vert", "ModelLoading.frag")) return;

	//delete factory
	delete factory;

	//Create Empty Node
	//renderObjects.push_back(unique_ptr<RenderObject>(new RenderObject()));
	//renderObjects[CurrentEmptyObjectIndex].get()->SetupModel("../resource/backpack.obj");
	//if(!renderObjects[CurrentEmptyObjectIndex].get()->SetupShader("ModelLoading.vert", "ModelLoading.frag")) return;
	glEnable(GL_DEPTH_TEST);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void JFRENDER_SYSTEM::SetupRenderObjectCubeScene()
{
	RenderObjectFactory_Cube* factory = new RenderObjectFactory_Cube();

	renderObjects.push_back(unique_ptr<RenderObject>(factory->CreateRenderObject("RenderObjectCube.vert", "RenderObjectCube.frag")));

	delete factory;
}

void JFRENDER_SYSTEM::CreateScene()
{
	// 1. Basic Cube Scene
	//SceneFactory_BasicCube* factory = new SceneFactory_BasicCube();

	// 2. Basic Model Scene
	//SceneFactory_BasicModel* factory = new SceneFactory_BasicModel();

	// 3.Directional lit cube
	SceneFactory_DirectionalLightCube * factory = new SceneFactory_DirectionalLightCube();

	//Create Scene
	CurrentScene = factory->CreateScene((float)Width, (float)Height, camera);

	//Delete factory
	delete factory;
}

FrameBuffer* JFRENDER_SYSTEM::GetFrameBufferByType(FRAME_BUFFER_TYPE Type)
{
	for (auto idx = 0; idx < FrameBuffers.size(); ++idx)
	{
		if (FrameBuffers[idx].get()->GetFrameBufferType() == Type)
		{
			return FrameBuffers[idx].get();
		}
	}
	return nullptr;
}

Shader* JFRENDER_SYSTEM::GetShaderByType(SHADER_TYPE _type)
{
	if (shaderManager == nullptr) return nullptr;
	Shader* shader = shaderManager->GetShaderByType(_type);
	if (shader == nullptr) return nullptr;
	return shader;
}
