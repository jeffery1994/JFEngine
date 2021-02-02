///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#include "camera.h"
#include <vector>
#include "RenderObject_Model.h"
#include "Scene.h"
#include "FrameBuffer.h"

struct GLFWwindow;
class RenderNode;
class Scene;
class ShaderManager;

static const unsigned int MAX_NUM_OF_SHADERS = 100;
static const unsigned int MAX_NUM_OF_RENDERNODE_PER_SCENE = 50;
static const unsigned int MAX_NUM_OF_CAMERAS = 3;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class JFRENDER_SYSTEM
{
public:
	JFRENDER_SYSTEM();
	~JFRENDER_SYSTEM();

	bool Init();
	void Deinit();
	void Update(float DeltaTime);
	bool ShouldExit();

	bool GetInited() { return Inited;  }
	GLFWwindow* GetWindow() { return Window;  }

	//Getter
	const unsigned int GetScreenQuadVAO() { return quadVAO; }
	int GetWindowWidth() { return Width; }
	int GetWindowHeight() { return Height; }
	Camera* GetActiveCamera() { return camera; }

	//Setup scenes
	void SetupCubeScene();
	void SetupCubeLightScene();
	void SetupCubeLightSceneWithMaterial();
	void SetupCubeLightSceneWithLightMaps();
	void SetupCubeLightSceneWithDirectionalLight();
	void SetupCubeLightSceneWithPointLight();
	void SetupCubeLightSceneWithSpotLight();
	void SetupCubeLightSceneWithMultipleLights();
	void SetupModelScene();
	void SetupRenderObjectCubeScene();

	//Setup Scene new
	void CreateScene();

	//
	FrameBuffer* GetFrameBufferByType(FRAME_BUFFER_TYPE Type);

private:
	bool InitGLFW();
	bool InitGLAD();
	void DeinitGLFW();
	void InitDefaultCamera();
	void InitScreenQuad();
	void InitShaderManager();

	//Node related functions
	void DeinitRenderNodes();
	void RenderRenderNodes();
	void InitRenderNodes();

	//Render object related function
	void DeinitRenderObjects();
	void RenderRenderObjects();
	void InitRenderObjects();

	//Buffer Related functions
	void InitFrameBuffers();
	void DeinitFrameBuffers();

	//Hacker function, TODO: delete this later
	void QuickSetupLightingCubeRenderNode();

	//frame buffer callback
	static void OnFrameBufferSizeChanged(GLFWwindow* window, int width, int height);

private:
	std::vector<std::unique_ptr<RenderObject>> renderObjects;
	std::vector<std::unique_ptr<FrameBuffer>> FrameBuffers;
	ShaderManager* shaderManager;
	Scene* CurrentScene;
	Camera* camera;
	GLFWwindow* Window;
	RenderNode* Nodes[MAX_NUM_OF_RENDERNODE_PER_SCENE];

	int CurrentEmptyNodeIndex = 0;
	unsigned int CurrentEmptyObjectIndex = 0;

	//Screen quad related parameters
	unsigned int quadVAO;
	unsigned int quadVBO;

	//Window parameters
	int Width = 800;
	int Height = 600;

	//Rendering techinques toggle
	bool usingDeferedShading = false;
	bool usingBloom = false;
	bool renderShadows = true;
	bool Inited = false;
};
