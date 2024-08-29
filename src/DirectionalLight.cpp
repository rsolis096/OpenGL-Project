#include "DirectionalLight.h"

bool firstFrame1 = true;

DirectionalLight::DirectionalLight(Shader* lightingShader, const glm::vec3& dir)
: m_LightingShader(lightingShader)
{
	//Used for rendering but not explicitly needed, just a GLM technicality
	m_LightPosition = glm::vec3( 5.0f, 10.0f, 5.0f);

	//Check if already normalized direction vector
	m_LightDirection = glm::normalize(glm::vec3(1.0f, 1.0f, 1.0f));
	m_DepthMapTexture = 0;
	m_DisplayName = "Directional Light";
	//Light color properties
	m_Ambient = glm::vec3(0.05f, 0.05f, 0.05f);
	m_Diffuse = glm::vec3(0.9f, 0.9f, 0.9f);
	m_Specular = glm::vec3(0.7f, 0.7f, 0.7f);


	m_LightingShader->use();
	m_LightingShader->setVec3("dirLight.position", m_LightPosition);
	m_LightingShader->setVec3("dirLight.direction", m_LightDirection);
	m_LightingShader->setVec3("dirLight.ambient", m_Ambient);
	m_LightingShader->setVec3("dirLight.diffuse", m_Diffuse);
	m_LightingShader->setVec3("dirLight.specular", m_Specular);
	m_LightingShader->setBool("hasDirLight", true);

	glCheckError();
}

void DirectionalLight::renderLight()
{

	//These only need to be initialized once
	//They are changed everytime a "set" method is called
	//Only set them after the render call.

	m_LightingShader->use();
	m_LightingShader->setVec3("dirLight.position", m_LightPosition);
	m_LightingShader->setVec3("dirLight.direction", m_LightDirection);
	m_LightingShader->setVec3("dirLight.ambient", m_Ambient);
	m_LightingShader->setVec3("dirLight.diffuse", m_Diffuse);
	m_LightingShader->setVec3("dirLight.specular", m_Specular);
	m_LightingShader->setBool("hasDirLight", true);


	// material properties
	m_LightingShader->setFloat("material.shininess", 32.0f);
	
}

DirectionalLight::~DirectionalLight()
{
	m_LightingShader = nullptr;
}

void DirectionalLight::setAmbient(glm::vec3 ambient)
{
	m_Ambient = ambient;
	m_LightingShader->use();
	m_LightingShader->setVec3("dirLight.ambient", m_Ambient);

}

void DirectionalLight::setDiffuse(glm::vec3 diffuse)
{
	m_Diffuse = diffuse;
	m_LightingShader->use();
	m_LightingShader->setVec3("dirLight.diffuse", m_Diffuse);
}

void DirectionalLight::setSpecular(glm::vec3 specular)
{
	m_Specular = specular;
	m_LightingShader->use();
	m_LightingShader->setVec3("dirLight.specular", m_Specular);
}

GLuint& DirectionalLight::getDepthMapTexture()
{
	return m_DepthMapTexture;
}