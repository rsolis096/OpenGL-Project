#include "SpotLight.h"

SpotLight::SpotLight(std::vector<float> inputVertices, std::vector<float> inputNormals, Shader& lightingShader, Shader& lightCubeShader, Camera& cam)
	: LightSource(cam)
{
	m_lightShapeShader = lightCubeShader;  
	m_lightingShader = lightingShader;
	m_LightShape = nullptr;
	playerCamera = cam;
	m_LightPos = playerCamera.cameraPos;

	//Light color properties
	m_Ambient = glm::vec3(0.0f, 0.0f, 0.0f);
	m_Diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
	m_Specular = glm::vec3(1.0f, 1.0f, 1.0f);

	//For Attenuation
	m_Constant = 1.0f;
	m_Linear = 0.09f;
	m_Quadratic = 0.032f;

	m_lightingShader.use();
	m_lightingShader.setVec3("spotLight.position", playerCamera.cameraPos);
	m_lightingShader.setVec3("spotLight.direction", playerCamera.cameraFront);
	m_lightingShader.setVec3("spotLight.ambient", m_Ambient);
	m_lightingShader.setVec3("spotLight.diffuse", m_Diffuse);
	m_lightingShader.setVec3("spotLight.specular", m_Specular);
	m_lightingShader.setFloat("spotLight.constant", m_Constant);
	m_lightingShader.setFloat("spotLight.linear", m_Linear);
	m_lightingShader.setFloat("spotLight.quadratic", m_Quadratic);
	m_lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
	m_lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

	// material properties
	m_lightingShader.setFloat("material.shininess", 32.0f);
}

void SpotLight::renderLight()
{
	//initializeLight();

	//Update the spotlights location (these stuff change every frame so always need to be updated
	m_lightingShader.use();
	m_lightingShader.setVec3("spotLight.position", playerCamera.cameraPos);
	m_lightingShader.setVec3("spotLight.direction", playerCamera.cameraFront);
	m_lightingShader.setVec3("viewPos", playerCamera.cameraPos);
	
	m_lightingShader.setVec3("spotLight.ambient", m_Ambient);
	m_lightingShader.setVec3("spotLight.diffuse", m_Diffuse);
	m_lightingShader.setVec3("spotLight.specular", m_Specular);
	m_lightingShader.setFloat("spotLight.constant", m_Constant);
	m_lightingShader.setFloat("spotLight.linear", m_Linear);
	m_lightingShader.setFloat("spotLight.quadratic", m_Quadratic);
	m_lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
	m_lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

	// material properties
	m_lightingShader.setFloat("material.shininess", 32.0f);
}

SpotLight::~SpotLight()
{
}

//For Light Properties
void SpotLight::setLightPos(glm::vec3 lightPos)
{
	m_LightPos = lightPos;
	m_lightingShader.use();
	m_lightingShader.setVec3("spotLight.position", m_LightPos);
}

void SpotLight::setAmbient(glm::vec3 ambient)
{
	m_Ambient = ambient;
	m_lightingShader.use();
	m_lightingShader.setVec3("spotLight.ambient", m_Ambient);

}

void SpotLight::setDiffuse(glm::vec3 diffuse)
{
	m_Diffuse = diffuse;
	m_lightingShader.use();
	m_lightingShader.setVec3("spotLight.diffuse", m_Diffuse);
}

void SpotLight::setSpecular(glm::vec3 specular)
{
	m_Specular = specular;
	m_lightingShader.use();
	m_lightingShader.setVec3("spotLight.specular", m_Specular);
}

//For Attenuation
void SpotLight::setconstant(float constant)
{
	m_Constant = constant;
	m_lightingShader.use();
	m_lightingShader.setFloat("spotLight.constant", m_Constant);
}

void SpotLight::setLinear(float linear)
{
	m_Linear = linear;
	m_lightingShader.use();
	m_lightingShader.setFloat("spotLight.linear", m_Linear);
}

void SpotLight::setQuadratic(float quadratic)
{
	m_Quadratic = quadratic;
	m_lightingShader.use();
	m_lightingShader.setFloat("spotLight.quadratic", m_Quadratic);
}
