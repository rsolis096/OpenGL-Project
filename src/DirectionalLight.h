#pragma once

#include <iostream>

#include "Shader.h"

class DirectionalLight
{
public:
	DirectionalLight(Shader* lightingShader, const glm::vec3& dir);
	void renderLight();
	~DirectionalLight();

	//For Light Properties
	void setAmbient(glm::vec3 ambient);
	void setDiffuse(glm::vec3 diffuse);
	void setSpecular(glm::vec3 specular);

	GLuint& getDepthMapTexture();

	glm::vec3 m_Ambient;
	glm::vec3 m_Diffuse;
	glm::vec3 m_Specular;
	glm::vec3 m_LightDirection;
	glm::vec3 m_LightPosition;

	std::string m_DisplayName;

private:
	Shader* m_LightingShader;

	GLuint m_DepthMapTexture;

};