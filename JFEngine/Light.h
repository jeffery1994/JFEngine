#pragma once
#include <glm/gtc/type_ptr.hpp>

enum LIGHT_TYPE
{
	DIRECTIONAL_LIGHT,
	POINT_LIGHT,
	SPOT_LIGHT,
};

class Light
{
public:
	Light();
	Light(
		glm::vec3 _position,
		glm::vec3 _ambient,
		glm::vec3 _diffuse,
		glm::vec3 _specular
	);
	Light(
		glm::vec3 _position, 
		glm::vec3 _direction, 
		glm::vec3 _ambient, 
		glm::vec3 _diffuse, 
		glm::vec3 _specular
	);
	~Light() {}

	//Getter
	inline const glm::vec3 GetPosition() { return position; }
	inline const glm::vec3 GetAmbient() { return ambient; }
	inline const glm::vec3 GetDiffuse() { return diffuse; }
	inline const glm::vec3 GetSpecular() { return specular; }
	inline const glm::vec3 GetDirection() { return direction; }
	inline const LIGHT_TYPE GetLightType() { return lightType; }

	//Setter
	inline void SetPosition(const glm::vec3 &_position) { this->position = _position; }
	inline void SetAmbient(const glm::vec3& _ambient) { this->ambient = _ambient; }
	inline void SetDiffuse(const glm::vec3& _diffuse) { this->diffuse = _diffuse; }
	inline void SetSpecular(const glm::vec3& _specular) { this->specular = _specular; }
	inline void SetDirection(const glm::vec3& _direction) { this->direction = _direction; }
	inline void SetLightType(const LIGHT_TYPE _lightType) { this->lightType = _lightType; }

	virtual void Setup(){}

protected:
	LIGHT_TYPE lightType = DIRECTIONAL_LIGHT;
	glm::vec3 direction;
	glm::vec3 position;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};

class PointLight : public Light
{
public:
	PointLight(
		glm::vec3 _position,
		glm::vec3 _ambient,
		glm::vec3 _diffuse,
		glm::vec3 _specular,
		float _constant,
		float _linear,
		float _quadratic
	);

	PointLight(
		glm::vec3 _position,
		glm::vec3 _ambient,
		glm::vec3 _diffuse,
		glm::vec3 _specular
	);

	//Specific getter
	inline const float GetConstant() { return constant; }
	inline const float GetLinear() { return linear; }
	inline const float GetQuadratic() { return quadratic; }

	//Specific setter
	void SetConstant(const float _constant) { this->constant = _constant; }
	void SetLinear(const float _linear) { this->linear = _linear; }
	void SetQuadratic(const float _quadratic) { this->quadratic = _quadratic; }

	//Virtual
	void Setup() {}
protected:
	float constant;
	float linear;
	float quadratic;
};

class SpotLight : public PointLight
{
public:
	SpotLight(glm::vec3 _position,
		glm::vec3 _ambient,
		glm::vec3 _diffuse,
		glm::vec3 _specular,
		float _cutOffAngleCosine = 12.5
	);

	//Getter
	inline const float GetCutOffAngleCosine() { return cutOffAngleCosine; }

	//Setter
	inline void SetCutOffAngleCosine(float _cutOffAngleCosine) { this->cutOffAngleCosine = _cutOffAngleCosine; }
private:
	float cutOffAngleCosine;
};