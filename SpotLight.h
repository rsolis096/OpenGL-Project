#pragma once

#include <iostream>
#include <vector>
#include <glad/glad.h>
#include "Object.h"
#include "Shader.h"
#include "Camera.h"

class SpotLight
{
public:
	SpotLight(Shader& lightingShader, Camera& cam);
	void renderLight();
	~SpotLight();

	//For Light Properties
	void setLightPos(glm::vec3 lightPos);
	void setAmbient(glm::vec3 ambient);
	void setDiffuse(glm::vec3 diffuse);
	void setSpecular(glm::vec3 specular);

	//For Attenuation
	void setconstant(float constant);
	void setLinear(float linear);
	void setQuadratic(float quadratic);

private:
	Shader m_lightingShader;
	Camera& playerCamera;
	glm::vec3 m_LightPos;

	//Light properties
	glm::vec3 m_Ambient;
	glm::vec3 m_Diffuse;
	glm::vec3 m_Specular;

	//For Attenuation
	float m_Constant;
	float m_Linear;
	float m_Quadratic;
};