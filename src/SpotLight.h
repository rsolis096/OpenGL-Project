#pragma once

#include <iostream>
#include <vector>
#include "Shader.h"
#include "Camera.h"
#include "Sphere.h"


class SpotLight
{
public:
	SpotLight(Shader*, Shader* ,Camera& );
	SpotLight(Shader*, Shader* ,glm::vec3 , glm::vec3 );
	void Draw();
	~SpotLight();

	//For Light Properties
	void setLightPos(glm::vec3);
	glm::vec3 getLightPos();
	void setLightDirection(glm::vec3);
	glm::vec3 getLightDirection();

	//For Attenuation
	void setconstant(float constant);
	void setLinear(float linear);
	void setQuadratic(float quadratic);

	//For Light Properties
	void setAmbient(glm::vec3 ambient);
	void setDiffuse(glm::vec3 diffuse);
	void setSpecular(glm::vec3 specular);

	//For Light Properties
	glm::vec3 getAmbient();
	glm::vec3 getDiffuse();
	glm::vec3 getSpecular();

	float getIntensity();
	void setIntensity(float i);
	std::string m_DisplayName;

	float getNearPlane();
	void setNearPlane(float&);
	float getFarPlane();
	void setFarPlane(float&);

private:
	Shader* m_lightingShader;
	Shader* m_ObjectShader;

	bool playerCamera;
	static int spotLightCount;
	int spotLightID;
	glm::vec3 m_lightDirection;
	glm::vec3 m_LightPos;
	Sphere* m_LightShape;

	glm::vec3 m_Ambient;
	glm::vec3 m_Diffuse;
	glm::vec3 m_Specular;

	//For Attenuation
	float m_Constant;
	float m_Linear;
	float m_Quadratic;
	float m_Intensity;

	//For shadows
	float m_NearPlane;
	float m_FarPlane;

};