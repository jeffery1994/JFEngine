///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#include "ShaderManager.h"
#include "Shader.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Shader* ShaderManager::GetShaderByType(SHADER_TYPE _shaderType)
{
	for (auto idx = 0; idx < Shaders.size(); ++idx)
	{
		if (Shaders[idx]->GetShaderType() == _shaderType)
		{
			return Shaders[idx];
		}
	}
	return nullptr;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ShaderManager::AddShaderByType(Shader* _shader, SHADER_TYPE _shaderType)
{
	_shader->SetShaderType(_shaderType);
	Shaders.push_back(_shader);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ShaderManager::RemoveShaderByType(SHADER_TYPE _shaderType)
{
	std::vector<Shader*>::iterator iterator;
	for (iterator = Shaders.begin(); iterator != Shaders.end(); ++iterator)
	{
		Shader* shader = *iterator;
		if (shader == nullptr) continue; //how?
		if (shader->GetShaderType() == _shaderType)
		{
			Shaders.erase(iterator);
			break;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ShaderManager::ClearShaders()
{
	for (auto idx = 0; idx < Shaders.size(); ++idx)
	{
		delete Shaders[idx];
	}
	Shaders.clear();
}