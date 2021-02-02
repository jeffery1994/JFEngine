///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include <glm/gtc/type_ptr.hpp>

//Opengl has a maximum of 16 bindable textures
static const int MAX_NUM_OF_TEXTURES = 16;

class Shader;


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class RenderNode
{
public:
	RenderNode();
	RenderNode(const char* vertexPath, const char* fragmentPath);
	~RenderNode();
	virtual void PreRender();
	virtual void Render();
	virtual void PostRender() {} //Unused for now
	virtual bool Setup() { return true; } //Unused for now
	void Destroy();
	Shader* GetShader() { return CurrentShader;  }

	//Transform
	void Rotate(float Degrees, glm::vec3 RotateAxis);
	void Scale(glm::vec3 scale);
	void Translate(glm::vec3 translate);
	void ResetTransform();

	//Render node setting up functions
	void SetupColorTexturedVerticesData(float vertices[], unsigned int numOfVertices, unsigned int numOfValueEachVertex);
	void SetupTexturedVerticesData(float Vertices[], unsigned int numOfVertices, unsigned int numOfValueEachVertex);
	void SetupVerticesTextureNormalData(float Vertices[], unsigned int numOfVertices, unsigned int numOfValueEachVertex);
	void SetupIndicesData(unsigned int indices[], unsigned int numOfIndices);
	bool SetupShaders(const char* vertexPath, const char* fragmentPath);
	bool SetupTextureByIndex(char const *texturePath, unsigned int TextureIndex, bool hasAlpha = false);
	void SetupTransformMetrix();
	void SetupModelMetrix();
	void SetupViewMetrix();
	void SetupProjectionMetrix();
	void SetupColor();

	//Setter
	void SetTransformMetrix(glm::mat4 Metrix) { TransformMetrix = Metrix; }
	void SetModelMetrix(glm::mat4 Metrix) { ModelMetrix = Metrix; }
	void SetViewMetrix(glm::mat4 Metrix) { ViewMetrix = Metrix; }
	void SetProjectionMetrix(glm::mat4 Metrix) { ProjectionMetrix = Metrix; }
	void SetPosition(glm::vec3 position);
	void SetColor(glm::vec4 color) { this->color = color; }

	//Getter
	glm::mat4 GetTransformMetrix() { return TransformMetrix; }
	glm::mat4 GetModelMetrix() { return ModelMetrix; }
	glm::mat4 GetViewMetrix() { return ViewMetrix; }
	glm::mat4 GetProjectionMetrix() { return ProjectionMetrix; }
	glm::vec4 GetColor() { return color; }
	glm::vec3 GetPosition() { return position; }
	unsigned int GetVBO() { return VBO; }
	unsigned int GetVAO() { return VAO; }
	unsigned int GetEBO() { return EBO; }
	unsigned int GetTextureByIndex(int Index) { return Textures[Index]; }

	//misc
	void ClearTextures();
protected:
	glm::mat4 TransformMetrix;
	glm::mat4 ModelMetrix;
	glm::mat4 ViewMetrix;
	glm::mat4 ProjectionMetrix;
	Shader* CurrentShader;

	glm::vec3 position;
	glm::vec4 color;

	unsigned int NumOfTexturesUsed = 0;
	unsigned int NumOfVertices = 0;
	unsigned int NumOfIndices = 0;
	unsigned int Textures[MAX_NUM_OF_TEXTURES];
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;
};