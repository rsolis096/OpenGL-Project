#include "SpotLight.h"

bool firstFrame2 = true;
int SpotLight::spotLightCount = 0;


SpotLight::SpotLight(Shader* lightingShader, Shader* objectShader, Camera& cam) :
	m_lightingShader(lightingShader), m_ObjectShader(objectShader), m_LightPos(cam.cameraPos), m_lightDirection(cam.cameraFront), playerCamera(true)
{
	spotLightID = spotLightCount;
	spotLightCount++;
	m_DisplayName = "SpotLight" + std::to_string(spotLightID);
	m_LightShape = nullptr;
	//Light color properties
	m_Ambient = glm::vec3(0.0f, 0.0f, 0.0f);
	m_Diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
	m_Specular = glm::vec3(1.0f, 1.0f, 1.0f);

	//For Attenuation (defaults)
	m_Constant = 1.0f;
	m_Linear = 0.09f;
	m_Quadratic = 0.032f;
	m_Intensity = 1;

	m_lightingShader->use();
	m_lightingShader->setInt("numberOfSpotLights", spotLightCount);
	m_lightingShader->setVec3("spotLights[" + std::to_string(spotLightID) + "].position", m_LightPos);
	m_lightingShader->setVec3("spotLights[" + std::to_string(spotLightID) + "].direction", m_lightDirection);
	m_lightingShader->setVec3("spotLights[" + std::to_string(spotLightID) + "].ambient", m_Ambient);
	m_lightingShader->setVec3("spotLights[" + std::to_string(spotLightID) + "].diffuse", m_Diffuse);
	m_lightingShader->setVec3("spotLights[" + std::to_string(spotLightID) + "].specular", m_Specular);
	m_lightingShader->setFloat("spotLights[" + std::to_string(spotLightID) + "].constant", m_Constant);
	m_lightingShader->setFloat("spotLights[" + std::to_string(spotLightID) + "].linear", m_Linear);
	m_lightingShader->setFloat("spotLights[" + std::to_string(spotLightID) + "].quadratic", m_Quadratic);
	m_lightingShader->setFloat("spotLights[" + std::to_string(spotLightID) + "].cutOff", glm::cos(glm::radians(12.5f)));
	m_lightingShader->setFloat("spotLights[" + std::to_string(spotLightID) + "].outerCutOff", glm::cos(glm::radians(15.0f)));;
}

SpotLight::SpotLight(Shader* lightingShader, Shader* objectShader, glm::vec3& pos, glm::vec3& dir) :
	m_lightingShader(lightingShader), m_ObjectShader(objectShader), m_LightPos(pos), m_lightDirection(dir), playerCamera(false)
{
	m_lightDirection = glm::normalize(dir - pos);
	spotLightID = spotLightCount;
	m_DisplayName = "SpotLight" + std::to_string(spotLightID);
	spotLightCount++;
	m_LightShape = new Sphere();
	m_LightShape->setPosition(pos);
	m_LightShape->setScale(glm::vec3(0.2f));
	//Light color properties
	m_Ambient = glm::vec3(1.0f, 1.0f, 1.0f);
	m_Diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
	m_Specular = glm::vec3(1.0f, 1.0f, 1.0f);

	//For Attenuation
	m_Constant = 1.0f;
	m_Linear = 0.09f;
	m_Quadratic = 0.032f;
	m_Intensity = 1;

	m_lightingShader->use();
	m_lightingShader->setInt("numberOfSpotLights", spotLightCount);
	m_lightingShader->setVec3("spotLights[" + std::to_string(spotLightID) + "].position", m_LightPos);
	m_lightingShader->setVec3("spotLights[" + std::to_string(spotLightID) + "].direction", m_lightDirection);
	m_lightingShader->setVec3("spotLights[" + std::to_string(spotLightID) + "].ambient", m_Ambient);
	m_lightingShader->setVec3("spotLights[" + std::to_string(spotLightID) + "].diffuse", m_Diffuse);
	m_lightingShader->setVec3("spotLights[" + std::to_string(spotLightID) + "].specular", m_Specular);
	m_lightingShader->setFloat("spotLights[" + std::to_string(spotLightID) + "].constant", m_Constant);
	m_lightingShader->setFloat("spotLights[" + std::to_string(spotLightID) + "].linear", m_Linear);
	m_lightingShader->setFloat("spotLights[" + std::to_string(spotLightID) + "].quadratic", m_Quadratic);
	m_lightingShader->setFloat("spotLights[" + std::to_string(spotLightID) + "].cutOff", glm::cos(glm::radians(12.5f)));
	m_lightingShader->setFloat("spotLights[" + std::to_string(spotLightID) + "].outerCutOff", glm::cos(glm::radians(15.0f)));;
}

void SpotLight::Draw()
{
	//If the spotlight is attached to the player than it will certainly change pos and dir every frame
	//other types of spotlights have their attibutes changes in setter functions
	if(playerCamera)
	{	
		m_lightingShader->use();
		m_lightingShader->setVec3("spotLights[" + std::to_string(spotLightID) + "].position", m_LightPos);
		m_lightingShader->setVec3("spotLights[" + std::to_string(spotLightID) + "].direction", m_lightDirection);
		m_lightingShader->setVec3("viewPos", m_LightPos);
	}
	if(m_LightShape != nullptr)
	{
		m_LightShape->Draw(*m_ObjectShader);
	}
}

SpotLight::~SpotLight()
{
}

//For Light Properties
void SpotLight::setLightPos(glm::vec3 lightPos)
{
	m_LightPos = lightPos;
	m_lightingShader->use();
	m_lightingShader->setVec3("spotLights[" + std::to_string(spotLightID) + "].position", m_LightPos);
	if(m_LightShape!=nullptr)
		m_LightShape->setPosition(lightPos);
}

void SpotLight::setAmbient(glm::vec3 ambient)
{
	m_Ambient = ambient * m_Intensity;
	m_LightShape->m_Ambient = m_Ambient;
	m_lightingShader->use();
	m_lightingShader->setVec3("spotLights[" + std::to_string(spotLightID) + "].ambient", m_Ambient);

}

void SpotLight::setDiffuse(glm::vec3 diffuse)
{
	m_Diffuse = diffuse * m_Intensity;
	m_LightShape->m_Diffuse = m_Diffuse;
	m_lightingShader->use();
	m_lightingShader->setVec3("spotLights[" + std::to_string(spotLightID) + "].diffuse", m_Diffuse);
}

void SpotLight::setSpecular(glm::vec3 specular)
{
	m_Specular = specular * m_Intensity;
	m_LightShape->m_Specular = m_Specular;
	m_lightingShader->use();
	m_lightingShader->setVec3("spotLights[" + std::to_string(spotLightID) + "].specular", m_Specular);
}

glm::vec3 SpotLight::getAmbient()
{
	return m_Ambient;
}

glm::vec3 SpotLight::getDiffuse()
{
	return m_Diffuse;
}

glm::vec3 SpotLight::getSpecular()
{
	return m_Specular;
}

float SpotLight::getIntensity()
{
	return m_Intensity;
}

void SpotLight::setIntensity(float i)
{
	m_Intensity = i;
}

//For Attenuation
void SpotLight::setconstant(float constant)
{
	m_Constant = constant;
	m_lightingShader->use();
	m_lightingShader->setFloat("spotLights[" + std::to_string(spotLightID) + "].constant", m_Constant);
}

void SpotLight::setLinear(float linear)
{
	m_Linear = linear;
	m_lightingShader->use();
	m_lightingShader->setFloat("spotLights[" + std::to_string(spotLightID) + "].linear", m_Linear);
}

void SpotLight::setQuadratic(float quadratic)
{
	m_Quadratic = quadratic;
	m_lightingShader->use();
	m_lightingShader->setFloat("spotLights[" + std::to_string(spotLightID) + "].quadratic", m_Quadratic);
}
