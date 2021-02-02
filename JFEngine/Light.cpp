#include "Light.h"

Light::Light()
{
	position = glm::vec3(0.0);
}

Light::Light(glm::vec3 _position, glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular)
	:position(_position)
	,ambient(_ambient)
	,diffuse(_diffuse)
	,specular(_specular)
{

}

Light::Light(glm::vec3 _position, glm::vec3 _direction, glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular)
	:position(_position)
	,direction(_direction)
	,ambient(_ambient)
	,diffuse(_diffuse)
	,specular(_specular)
{
	position = _position;
}

PointLight::PointLight(glm::vec3 _position, glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, float _constant, float _linear, float _quadratic)
	:Light(_position, _ambient, _diffuse, _specular)
	,constant(_constant)
	,linear(_linear)
	,quadratic(_quadratic)
{
	lightType = POINT_LIGHT;
}

PointLight::PointLight(glm::vec3 _position, glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular)
	:Light(_position, _ambient, _diffuse, _specular)
{
	constant = 1.0f;
	linear = 0.022f;
	quadratic = 0.0019f;
	lightType = POINT_LIGHT;
}

SpotLight::SpotLight(glm::vec3 _position, glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, float _cutOffAngleCosine)
	:PointLight(_position, _ambient, _diffuse, _specular)
	,cutOffAngleCosine(_cutOffAngleCosine)
{
	lightType = SPOT_LIGHT;
}
