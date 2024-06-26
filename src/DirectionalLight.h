#pragma once

#include <iostream>

#include "Shader.h"

class DirectionalLight
{
public:
	DirectionalLight(Shader* lightingShader);
	void renderLight();
	~DirectionalLight();

	//For Light Properties
	void setAmbient(glm::vec3 ambient);
	void setDiffuse(glm::vec3 diffuse);
	void setSpecular(glm::vec3 specular);
	glm::vec3 m_Ambient;
	glm::vec3 m_Diffuse;
	glm::vec3 m_Specular;

private:
	Shader* m_lightingShader;
	glm::vec3 m_direction;
};