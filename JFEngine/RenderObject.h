///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#pragma once
#include <memory>
#include "Model.h"
#include "Shader.h"
#include <glm/gtc/type_ptr.hpp>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class RenderObject
{
public:
	RenderObject();
	~RenderObject() {}

	RenderObject(const char* vertexPath, const char* fragmentPath);
	RenderObject(const char* vertexPath, const char* fragmentPath, const char* outlineVertPath, const char* outlineFragPath);

	bool SetupShader(const char* vertexShaderPath, const char* fragmentShaderPath);
	bool SetupShadowShader(const char* _shadowVertShader, const char* _shadowFrag);
	bool SetupDeferedShader(const char* _deferedVertShader, const char* _deferedFragShader);
	virtual void SetupRenderObject(const char* modelPath = "") {}

	virtual void PreRender();
	virtual void PreRender(Shader* _shader);
	virtual void Render();
	virtual void Render(Shader* _shader);
	virtual void PostRender() {}
	virtual void ShadowPass();
	virtual void Destroy();
	virtual void Draw();
	virtual void Draw(Shader* _shader);
	virtual void DrawWithOutline();

	//Transform related functions
	void Rotate(float Degrees, glm::vec3 RotateAxis);
	void Scale(glm::vec3 scale);
	void Translate(glm::vec3 translate);

	//Getter
	inline glm::mat4 GetModelMetrix() { return ModelMetrix; }
	inline glm::mat4 GetViewMetrix() { return ViewMetrix; }
	inline glm::mat4 GetProjectionMetrix() { return ProjectionMetrix; }
	inline Shader* GetShader() { return shader.get(); }
	inline Shader* const GetShadowShader() { return shadowShader.get(); }
	inline Shader* const GetDeferedShader() { return deferedShader.get(); }
	inline bool GetDrawOutline() { return drawOutline; }
	virtual const unsigned int GetNumberOfTexture() { return 0; }

	//Setter
	inline void SetModelMetrix(glm::mat4 Metrix) { ModelMetrix = Metrix; }
	inline void SetViewMetrix(glm::mat4 Metrix) { ViewMetrix = Metrix; }
	inline void SetProjectionMetrix(glm::mat4 Metrix) { ProjectionMetrix = Metrix; }
	inline void SetDrawOutline(bool _drawOutline) { drawOutline = _drawOutline; }

	//Texture
	virtual bool AddTexture(Texture& texture);
	virtual bool AddCubeMapTexture(std::vector<Texture>& _texture) { return true; }
protected:
	std::unique_ptr<Shader> shader;
	std::unique_ptr<Shader> shadowShader;
	std::unique_ptr<Shader> outlineShader;
	std::unique_ptr<Shader> deferedShader;
	glm::mat4 ModelMetrix;
	glm::mat4 ViewMetrix;
	glm::mat4 ProjectionMetrix;
	bool drawOutline;
private:
	void ResetMetrix();
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class RenderObjectFactory
{
public:
	virtual RenderObject* CreateRenderObject() = 0;
	virtual RenderObject* CreateRenderObject(const char* vertexPath, const char* fragmentPath) = 0;
};