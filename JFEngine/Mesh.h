///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#pragma once
#include <string>
#include <vector>
#include <glm/gtc/type_ptr.hpp>
using namespace std;

class Shader;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
enum SIMPLE_MESH_TYPE
{
	NONE,
	CUBE,
	QUAD,
	SKYBOX,
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
struct Vertex {
	// position
	glm::vec3 Position;
	// normal
	glm::vec3 Normal;
	// texCoords
	glm::vec2 TexCoords;
	// tangent
	glm::vec3 Tangent;
	// bitangent
	glm::vec3 Bitangent;

	Vertex() {}
	Vertex(float p1, float p2, float p3, float n1, float n2, 
		float n3, float t1, float t2, float ta1, float ta2, float ta3,
		float bta1, float bta2, float bta3)
	{
		Position = glm::vec3(p1, p2, p3);
		Normal = glm::vec3(n1, n2, n3);
		TexCoords = glm::vec2(t1, t2);
		Tangent = glm::vec3(ta1, ta2, ta3);
		Bitangent = glm::vec3(bta1, bta2, bta3);
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
struct Texture {
	unsigned int id;
	string type;
	string path;

	Texture() {}
	Texture(unsigned int id, string _type, string _path)
	{
		this->id = id;
		type = _type;
		path = _path;
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class Mesh {
public:
	// mesh Data
	vector<Vertex>       vertices;
	vector<unsigned int> indices;
	vector<Texture>      textures;
	unsigned int VAO;

	// constructor
	Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);
	Mesh(SIMPLE_MESH_TYPE Type);

	// render the mesh
	void Draw(Shader* shader);
	void Draw();

	// Add texture to mesh
	bool AddTexture(Texture &texture);

	// Add cube map texture
	bool AddCubeMapTexture(std::vector<Texture>& _textures);

	void BindTexture(unsigned int _tbo, const char* name);
	inline unsigned int GetCubeMap() { return cubeMap; }
	inline const bool GetHasCubeMap() { return hasCubeMap; }
	const int GetNumberOfTextures() { 
		if (hasCubeMap) return (const int)textures.size() + 1;
		return (const int)textures.size();
	}

protected:
	// render data 
	unsigned int VBO, EBO;

	// cube map texture object
	unsigned int cubeMap;

	// initializes all the buffer objects/arrays
	virtual void SetupMesh();

	unsigned int shadowMap;
private:
	SIMPLE_MESH_TYPE type;
	bool hasCubeMap;
};