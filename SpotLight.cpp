#include "SpotLight.h"

SpotLight::SpotLight(std::vector<float> inputVertices, std::vector<float> inputNormals,
	Shader& lightingShader, Shader& lightCubeShader, Camera& cam) : LightSource(inputVertices, inputNormals,
		lightingShader, lightCubeShader, cam)
{
	m_Ambient = glm::vec3(0.1f, 0.1f, 0.1f); //Dark ambient
	m_Diffuse = glm::vec3(0.8f, 0.8f, 0.8f); //Grey light color
	m_Specular = glm::vec3(1.0f, 1.0f, 1.0f); //"Shine" color

	//All lights will have the same initial drop off
	m_Constant = 1.0f;
	m_Linear = 0.09f;
	m_Quadratic = 0.032f;
}

void SpotLight::renderLight()
{
	initializeLight();
	//Update the spotlights location
	m_lightingShader.use();
	m_lightingShader.setVec3("light.position", playerCamera.cameraPos);
	m_lightingShader.setVec3("light.direction", playerCamera.cameraFront);
	m_lightingShader.setVec3("viewPos", playerCamera.cameraPos);
}

SpotLight::~SpotLight()
{
}

void SpotLight::initializeLight()
{
	//Initialze Light properties, these can be changed at run time
	m_lightingShader.use();

	//Light type
	m_lightingShader.setInt("lightFlag", 2);

	//Light fall off
	m_lightingShader.setFloat("light.constant", m_Constant);
	m_lightingShader.setFloat("light.linear", m_Linear);
	m_lightingShader.setFloat("light.quadratic", m_Quadratic);

	//Light Color Properties
	m_lightingShader.setVec3("light.ambient", m_Ambient);
	m_lightingShader.setVec3("light.diffuse", m_Diffuse);
	m_lightingShader.setVec3("light.specular", m_Specular);

	m_lightingShader.setFloat("light.cutOff", glm::cos(glm::radians(12.5f)));
	m_lightingShader.setFloat("light.outerCutOff", glm::cos(glm::radians(17.5f)));

	// material properties
	m_lightingShader.setFloat("material.shininess", 32.0f);
}

