#include "SpotLight.h"

bool firstFrame2 = true;
int SpotLight::spotLightCount = 0;


SpotLight::SpotLight(Shader& lightingShader, Camera& cam) :
	m_lightingShader(lightingShader), m_LightPos(cam.cameraPos), m_lightDirection(cam.cameraFront), playerCamera(true)
{
	spotLightID = spotLightCount;
	spotLightCount++;
	//Light color properties
	m_Ambient = glm::vec3(0.0f, 0.0f, 0.0f);
	m_Diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
	m_Specular = glm::vec3(1.0f, 1.0f, 1.0f);

	//For Attenuation
	m_Constant = 1.0f;
	m_Linear = 0.09f;
	m_Quadratic = 0.032f;

	m_lightingShader.use();
	m_lightingShader.setInt("numberOfSpotLights", spotLightCount);
	m_lightingShader.setVec3("spotLights[" + std::to_string(spotLightID) + "].position", m_LightPos);
	m_lightingShader.setVec3("spotLights[" + std::to_string(spotLightID) + "].direction", m_lightDirection);
	m_lightingShader.setVec3("spotLights[" + std::to_string(spotLightID) + "].ambient", m_Ambient);
	m_lightingShader.setVec3("spotLights[" + std::to_string(spotLightID) + "].diffuse", m_Diffuse);
	m_lightingShader.setVec3("spotLights[" + std::to_string(spotLightID) + "].specular", m_Specular);
	m_lightingShader.setFloat("spotLights[" + std::to_string(spotLightID) + "].constant", m_Constant);
	m_lightingShader.setFloat("spotLights[" + std::to_string(spotLightID) + "].linear", m_Linear);
	m_lightingShader.setFloat("spotLights[" + std::to_string(spotLightID) + "].quadratic", m_Quadratic);
	m_lightingShader.setFloat("spotLights[" + std::to_string(spotLightID) + "].cutOff", glm::cos(glm::radians(12.5f)));
	m_lightingShader.setFloat("spotLights[" + std::to_string(spotLightID) + "].outerCutOff", glm::cos(glm::radians(15.0f)));;
}

SpotLight::SpotLight(Shader& lightingShader, glm::vec3& pos, glm::vec3& dir) : 
	m_lightingShader(lightingShader), m_LightPos(pos), m_lightDirection(dir), playerCamera(false)
{
	m_lightDirection = -glm::normalize(dir - pos);
	spotLightID = spotLightCount;
	spotLightCount++;
	//Light color properties
	m_Ambient = glm::vec3(0.1f, 0.1f, 0.1f);
	m_Diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
	m_Specular = glm::vec3(1.0f, 1.0f, 1.0f);

	//For Attenuation
	m_Constant = 1.0f;
	m_Linear = 0.09f;
	m_Quadratic = 0.032f;

	m_lightingShader.use();
	m_lightingShader.setInt("numberOfSpotLights", spotLightCount);
	m_lightingShader.setVec3("spotLights[" + std::to_string(spotLightID) + "].position", m_LightPos);
	m_lightingShader.setVec3("spotLights[" + std::to_string(spotLightID) + "].direction", m_lightDirection);
	m_lightingShader.setVec3("spotLights[" + std::to_string(spotLightID) + "].ambient", m_Ambient);
	m_lightingShader.setVec3("spotLights[" + std::to_string(spotLightID) + "].diffuse", m_Diffuse);
	m_lightingShader.setVec3("spotLights[" + std::to_string(spotLightID) + "].specular", m_Specular);
	m_lightingShader.setFloat("spotLights[" + std::to_string(spotLightID) + "].constant", m_Constant);
	m_lightingShader.setFloat("spotLights[" + std::to_string(spotLightID) + "].linear", m_Linear);
	m_lightingShader.setFloat("spotLights[" + std::to_string(spotLightID) + "].quadratic", m_Quadratic);
	m_lightingShader.setFloat("spotLights[" + std::to_string(spotLightID) + "].cutOff", glm::cos(glm::radians(12.5f)));
	m_lightingShader.setFloat("spotLights[" + std::to_string(spotLightID) + "].outerCutOff", glm::cos(glm::radians(15.0f)));;
}

void SpotLight::renderLight()
{
	//initializeLight();

	//Update the spotlights location (these stuff change every frame so always need to be updated
	m_lightingShader.use();
	m_lightingShader.setVec3("spotLights[" + std::to_string(spotLightID) + "].position", m_LightPos);
	m_lightingShader.setVec3("spotLights[" + std::to_string(spotLightID) + "].direction", m_lightDirection);
	if(playerCamera)
		m_lightingShader.setVec3("viewPos", m_LightPos);
}

SpotLight::~SpotLight()
{
}

//For Light Properties
void SpotLight::setLightPos(glm::vec3 lightPos)
{
	m_LightPos = lightPos;
	m_lightingShader.use();
	m_lightingShader.setVec3("spotLights[" + std::to_string(spotLightID) + "].position", m_LightPos);
}

void SpotLight::setAmbient(glm::vec3 ambient)
{
	m_Ambient = ambient;
	m_lightingShader.use();
	m_lightingShader.setVec3("spotLights[" + std::to_string(spotLightID) + "].ambient", m_Ambient);

}

void SpotLight::setDiffuse(glm::vec3 diffuse)
{
	m_Diffuse = diffuse;
	m_lightingShader.use();
	m_lightingShader.setVec3("spotLights[" + std::to_string(spotLightID) + "].diffuse", m_Diffuse);
}

void SpotLight::setSpecular(glm::vec3 specular)
{
	m_Specular = specular;
	m_lightingShader.use();
	m_lightingShader.setVec3("spotLights[" + std::to_string(spotLightID) + "].specular", m_Specular);
}

//For Attenuation
void SpotLight::setconstant(float constant)
{
	m_Constant = constant;
	m_lightingShader.use();
	m_lightingShader.setFloat("spotLights[" + std::to_string(spotLightID) + "].constant", m_Constant);
}

void SpotLight::setLinear(float linear)
{
	m_Linear = linear;
	m_lightingShader.use();
	m_lightingShader.setFloat("spotLights[" + std::to_string(spotLightID) + "].linear", m_Linear);
}

void SpotLight::setQuadratic(float quadratic)
{
	m_Quadratic = quadratic;
	m_lightingShader.use();
	m_lightingShader.setFloat("spotLights[" + std::to_string(spotLightID) + "].quadratic", m_Quadratic);
}
