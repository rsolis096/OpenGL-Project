#include "DirectionalLight.h"

bool firstFrame1 = true;

DirectionalLight::DirectionalLight(Shader* lightingShader)
{
	m_lightingShader = lightingShader;

	//Light color properties
	m_direction = glm::vec3(0.0f, -1.0f, 0.0f);
	m_Ambient = glm::vec3(0.1f, 0.1f, 0.1f);
	m_Diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
	m_Specular = glm::vec3(0.5f, 0.5f, 0.5f);

	glCheckError();

}

void DirectionalLight::renderLight()
{

	//These only need to be initialized once
	//They are changed everytime a "set" method is called
	//Only set them after the render call.
	if (firstFrame1)
	{
		m_lightingShader->use();
		m_lightingShader->setVec3("dirLight.direction", m_direction);
		m_lightingShader->setVec3("dirLight.ambient", m_Ambient);
		m_lightingShader->setVec3("dirLight.diffuse", m_Diffuse);
		m_lightingShader->setVec3("dirLight.specular", m_Specular);
		// material properties
		m_lightingShader->setFloat("material.shininess", 32.0f);
		firstFrame1 = false;
	}
}

DirectionalLight::~DirectionalLight()
{

}

void DirectionalLight::setAmbient(glm::vec3 ambient)
{
	m_Ambient = ambient;
	m_lightingShader->use();
	m_lightingShader->setVec3("dirLight.ambient", m_Ambient);

}

void DirectionalLight::setDiffuse(glm::vec3 diffuse)
{
	m_Diffuse = diffuse;
	m_lightingShader->use();
	m_lightingShader->setVec3("dirLight.diffuse", m_Diffuse);
}

void DirectionalLight::setSpecular(glm::vec3 specular)
{
	m_Specular = specular;
	m_lightingShader->use();
	m_lightingShader->setVec3("dirLight.specular", m_Specular);
}