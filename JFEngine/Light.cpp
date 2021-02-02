///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#include "Light.h"
#include <glm/gtc/matrix_transform.hpp> 

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Light::Light()
{
	position = glm::vec3(0.0);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Light::Light(glm::vec3 _position, glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular)
	:position(_position)
	,ambient(_ambient)
	,diffuse(_diffuse)
	,specular(_specular)
{
	SetupMatrix();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Light::Light(glm::vec3 _position, glm::vec3 _direction, glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular)
	:position(_position)
	,direction(_direction)
	,ambient(_ambient)
	,diffuse(_diffuse)
	,specular(_specular)
{
	position = _position;
	SetupMatrix();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Light::SetupMatrix()
{
	float near_plane = 0.1f;
	float far_plane = 100.0f;
	lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
	lightView = glm::lookAt(-direction, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	lightSpaceMatrix = lightProjection * lightView;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
PointLight::PointLight(glm::vec3 _position, glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, float _constant, float _linear, float _quadratic)
	:Light(_position, _ambient, _diffuse, _specular)
	,constant(_constant)
	,linear(_linear)
	,quadratic(_quadratic)
{
	lightType = POINT_LIGHT;
	SetupMatrix();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
PointLight::PointLight(glm::vec3 _position, glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular)
	:Light(_position, _ambient, _diffuse, _specular)
{
	constant = 1.0f;
	linear = 0.022f;
	quadratic = 0.0019f;
	lightType = POINT_LIGHT;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void PointLight::SetupMatrix()
{
	float near_plane = 0.1f;
	float far_plane = 100.0f;
	lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
	lightView = glm::lookAt(-direction, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	lightSpaceMatrix = lightProjection * lightView;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SpotLight::SpotLight(glm::vec3 _position, glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, float _cutOffAngleCosine, float _outterCutoff)
	:PointLight(_position, _ambient, _diffuse, _specular)
	,cutOffAngleCosine(glm::cos(glm::radians(_cutOffAngleCosine)))
	,outterCutoffAngle(glm::cos(glm::radians(_outterCutoff)))
{
	lightType = SPOT_LIGHT;
	SetupMatrix();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void SpotLight::SetupMatrix()
{
	float near_plane = 0.1f;
	float far_plane = 100.0f;
	lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
	lightView = glm::lookAt(-direction, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	lightSpaceMatrix = lightProjection * lightView;
}
