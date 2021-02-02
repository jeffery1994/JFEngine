///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#include "RenderNode.h"
#include "Shader.h"
#include "JFOpenGL.h"
//#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
//#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RenderNode::RenderNode()
	:CurrentShader(nullptr)
	,NumOfTexturesUsed(0)
	,NumOfVertices(0)
	,VAO(0)
	,VBO(0)
	,EBO(0)
{
	color = glm::vec4(0.0, 0.0, 0.0, 1);
	SetPosition(glm::vec3(0.0,0.0,0.0));
	SetModelMetrix(glm::mat4(1.0f));
	SetViewMetrix(glm::mat4(1.0f));
	SetProjectionMetrix(glm::mat4(1.0f));
	ClearTextures();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RenderNode::RenderNode(const char* vertexPath, const char* fragmentPath)
	:CurrentShader(nullptr)
	, NumOfTexturesUsed(0)
	, NumOfVertices(0)
	, VAO(0)
	, VBO(0)
	, EBO(0)
{
	SetupShaders(vertexPath, fragmentPath);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RenderNode::~RenderNode()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RenderNode::Destroy()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteProgram(CurrentShader->ID);
	delete CurrentShader;
	CurrentShader = nullptr;
	NumOfVertices = 0;
	NumOfTexturesUsed = 0;
	VAO = 0;
	VBO = 0;
	EBO = 0;
	ClearTextures();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RenderNode::ClearTextures()
{
	for (unsigned int idx = 0; idx < MAX_NUM_OF_TEXTURES; ++idx)
	{
		Textures[idx] = 0;
	}
	NumOfTexturesUsed = 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RenderNode::SetupTransformMetrix()
{
	unsigned int transformLoc = glGetUniformLocation(CurrentShader->ID, "transform");
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(TransformMetrix));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RenderNode::SetupModelMetrix()
{
	unsigned int loc = glGetUniformLocation(CurrentShader->ID, "model");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(TransformMetrix));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RenderNode::SetupViewMetrix()
{
	unsigned int loc = glGetUniformLocation(CurrentShader->ID, "view");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(ViewMetrix));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RenderNode::SetupProjectionMetrix()
{
	unsigned int loc = glGetUniformLocation(CurrentShader->ID, "projection");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(ProjectionMetrix));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RenderNode::SetupColor()
{
	unsigned int loc = glGetUniformLocation(CurrentShader->ID, "color");
	glUniform4fv(loc, 1, glm::value_ptr(this->color));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RenderNode::PreRender()
{
	CurrentShader->use();
	//SetupTransformMetrix();
	SetupModelMetrix();
	SetupViewMetrix();
	SetupProjectionMetrix();
	SetupColor();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RenderNode::Render()
{
	// bind textures on corresponding texture units
	for (unsigned int idx = 0; idx < NumOfTexturesUsed; ++idx)
	{
		glActiveTexture(GL_TEXTURE0 + idx);
		glBindTexture(GL_TEXTURE_2D, Textures[idx]);
	}

	// render container
	//CurrentShader->use();
	glBindVertexArray(VAO);
	if (NumOfIndices != 0)
	{
		glDrawElements(GL_TRIANGLES, NumOfIndices, GL_UNSIGNED_INT, 0);
	}
	else
	{
		glDrawArrays(GL_TRIANGLES, 0, NumOfVertices);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool RenderNode::SetupShaders(const char* vertexPath, const char* fragmentPath)
{
	CurrentShader = new Shader(vertexPath, fragmentPath);
	if (!CurrentShader->GetInited()) return false;
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RenderNode::SetupColorTexturedVerticesData(float vertices[], unsigned int numOfVertices, unsigned int numOfValueEachVertex)
{
	// Generate buffer objects
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// Bind vertex array objects
	glBindVertexArray(VAO);

	// Bind vertex buffer object and buffer data
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * numOfValueEachVertex * numOfVertices, vertices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	NumOfVertices = numOfVertices;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RenderNode::SetupTexturedVerticesData(float vertices[], unsigned int numOfVertices, unsigned int numOfValueEachVertex)
{
	// Generate buffer objects
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// Bind vertex array objects
	glBindVertexArray(VAO);

	// Bind vertex buffer object and buffer data
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * numOfValueEachVertex * numOfVertices, vertices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// texture coordinates attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	NumOfVertices = numOfVertices;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RenderNode::SetupVerticesTextureNormalData(float vertices[], unsigned int numOfVertices, unsigned int numOfValueEachVertex)
{
	// Generate buffer objects
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// Bind vertex array objects
	glBindVertexArray(VAO);

	// Bind vertex buffer object and buffer data
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * numOfValueEachVertex * numOfVertices, vertices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// texture coordinates attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// normal coordinates attributes
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);

	NumOfVertices = numOfVertices;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RenderNode::SetupIndicesData(unsigned int indices[], unsigned int numOfIndices)
{
	//Generate element buffer objects
	glGenBuffers(1, &EBO);

	// Bind element buffer objects and buffer data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * numOfIndices, indices, GL_STATIC_DRAW);

	NumOfIndices = numOfIndices;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool RenderNode::SetupTextureByIndex(char const *texturePath, unsigned int TextureIndex, bool hasAlpha)
{
	glGenTextures(1, &Textures[TextureIndex]);
	glBindTexture(GL_TEXTURE_2D, Textures[TextureIndex]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	// tell stb_image.h to flip loaded texture's on the y-axis.
	stbi_set_flip_vertically_on_load(true); 
	unsigned char *data = stbi_load(texturePath, &width, &height, &nrChannels, 0);
	if (data)
	{
		if (!hasAlpha && nrChannels == 3)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
		return false;
	}
	stbi_image_free(data);
	NumOfTexturesUsed += 1;
	return true;
}

///////////////////////////////////////////////////////////////////////////////
//
// Transform
//
///////////////////////////////////////////////////////////////////////////////
void RenderNode::Rotate(float Degrees, glm::vec3 RotateAxis)
{
	TransformMetrix = glm::rotate(TransformMetrix, glm::radians(Degrees), RotateAxis);
}

void RenderNode::Scale(glm::vec3 scale)
{
	TransformMetrix = glm::scale(TransformMetrix, scale);
}

void RenderNode::Translate(glm::vec3 translate)
{
	position = position + translate;
	TransformMetrix = glm::translate(TransformMetrix, this->position);
}

void RenderNode::SetPosition(glm::vec3 position)
{
	this->position = position;
	TransformMetrix = glm::translate(TransformMetrix, this->position);
}