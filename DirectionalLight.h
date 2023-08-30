#pragma once

#include <iostream>
#include <glad/glad.h>
#include "Shader.h"
#include "Camera.h"

class DirectionalLight
{
public:
	DirectionalLight(Shader& lightingShader);
	void renderLight();
	~DirectionalLight();

	//For Light Properties
	void setAmbient(glm::vec3 ambient);
	void setDiffuse(glm::vec3 diffuse);
	void setSpecular(glm::vec3 specular);

private:
	Shader m_lightingShader;
	glm::vec3 m_direction;

	//Light properties
	glm::vec3 m_Ambient;
	glm::vec3 m_Diffuse;
	glm::vec3 m_Specular;
};