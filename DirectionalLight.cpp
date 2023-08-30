#include "DirectionalLight.h"

DirectionalLight::DirectionalLight(Shader& lightingShader, Camera& cam) : playerCamera(cam)
{
	m_lightingShader = lightingShader;

	//Light color properties
	m_direction = glm::vec3(-0.2f, -1.0f, -0.3f);
	m_Ambient = glm::vec3(0.25f, 0.25f, 0.25f);
	m_Diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
	m_Specular = glm::vec3(0.5f, 0.5f, 0.5f);

	m_lightingShader.use();
	m_lightingShader.setVec3("dirLight.direction", m_direction);
	m_lightingShader.setVec3("dirLight.ambient", m_Ambient);
	m_lightingShader.setVec3("dirLight.diffuse", m_Diffuse);
	m_lightingShader.setVec3("dirLight.specular", m_Specular);

}

void DirectionalLight::renderLight()
{
	//initializeLight();

	//Update the DirectionalLights location (these stuff change every frame so always need to be updated
	m_lightingShader.use();
	m_lightingShader.setVec3("viewPos", playerCamera.cameraPos);
	m_lightingShader.setVec3("dirLight.direction", m_direction);
	m_lightingShader.setVec3("dirLight.ambient", m_Ambient);
	m_lightingShader.setVec3("dirLight.diffuse", m_Diffuse);
	m_lightingShader.setVec3("dirLight.specular", m_Specular);
	// material properties
	m_lightingShader.setFloat("material.shininess", 32.0f);
}

DirectionalLight::~DirectionalLight()
{
}

void DirectionalLight::setAmbient(glm::vec3 ambient)
{
	m_Ambient = ambient;
	m_lightingShader.use();
	m_lightingShader.setVec3("dirLight.ambient", m_Ambient);

}

void DirectionalLight::setDiffuse(glm::vec3 diffuse)
{
	m_Diffuse = diffuse;
	m_lightingShader.use();
	m_lightingShader.setVec3("dirLight.diffuse", m_Diffuse);
}

void DirectionalLight::setSpecular(glm::vec3 specular)
{
	m_Specular = specular;
	m_lightingShader.use();
	m_lightingShader.setVec3("dirLight.specular", m_Specular);
}