#include "SpotLight.h"

bool firstFrame2 = true;
unsigned short SpotLight::m_SpotLightCount = 0;

SpotLight::SpotLight(Shader* lightingShader, Shader* lightSourceShader, glm::vec3 pos, glm::vec3 dir) :
	m_LightingShader(lightingShader), m_LightSourceShader(lightSourceShader),
	m_LightPos(pos), m_LightDirection(dir),
	m_NearPlane(0.5f), m_FarPlane(70.0f)
{
	m_LightDirection = glm::normalize(dir);

	m_SpotLightID = m_SpotLightCount;
	m_DisplayName = "SpotLight" + std::to_string(m_SpotLightID);
	m_SpotLightCount+= 1;

	//m_SpotLight takes the shape of a sphere
	m_LightShape = new Sphere();
	m_LightShape->setPosition(pos);
	m_LightShape->setScale(glm::vec3(0.2f));

	//Light color properties
	m_Intensity = 15.0f;
	m_Ambient = glm::vec3(1.0f, 1.0f, 1.0f) * m_Intensity;
	m_Diffuse = glm::vec3(1.0f, 1.0f, 1.0f) * m_Intensity;
	m_Specular = glm::vec3(1.0f, 1.0f, 1.0f) * m_Intensity;

	//For Attenuation
	m_Constant = 1.0f;
	m_Linear = 0.09f;
	m_Quadratic = 0.032f;
	
	m_LightingShader->use();
	m_LightingShader->setInt("numberOfSpotLightsFRAG", m_SpotLightCount);
	m_LightingShader->setInt("numberOfSpotLightsVERT", m_SpotLightCount);
	m_LightingShader->setVec3("spotLights[" + std::to_string(m_SpotLightID) + "].position", m_LightPos);
	m_LightingShader->setVec3("spotLights[" + std::to_string(m_SpotLightID) + "].direction", m_LightDirection);
	m_LightingShader->setVec3("spotLights[" + std::to_string(m_SpotLightID) + "].ambient", m_Ambient);
	m_LightingShader->setVec3("spotLights[" + std::to_string(m_SpotLightID) + "].diffuse", m_Diffuse);
	m_LightingShader->setVec3("spotLights[" + std::to_string(m_SpotLightID) + "].specular", m_Specular);
	m_LightingShader->setFloat("spotLights[" + std::to_string(m_SpotLightID) + "].constant", m_Constant);
	m_LightingShader->setFloat("spotLights[" + std::to_string(m_SpotLightID) + "].linear", m_Linear);
	m_LightingShader->setFloat("spotLights[" + std::to_string(m_SpotLightID) + "].quadratic", m_Quadratic);
	m_LightingShader->setFloat("spotLights[" + std::to_string(m_SpotLightID) + "].cutOff", glm::cos(glm::radians(12.5f)));
	m_LightingShader->setFloat("spotLights[" + std::to_string(m_SpotLightID) + "].outerCutOff", glm::cos(glm::radians(15.0f)));
}

SpotLight::~SpotLight()
{
	//TODO: Implement Rule of 3 and 5 if you want
	m_LightingShader = nullptr;
	m_LightSourceShader = nullptr;
	delete m_LightShape;
	m_LightShape = nullptr;
}

void SpotLight::Draw() const
{
	m_LightingShader->use();
	m_LightingShader->setInt("lightType", 1);
	if(m_LightShape != nullptr)
		m_LightShape->Draw(*m_LightSourceShader);
}

/*###################################
* ###     SETTER FUNCTIONS        ###
###################################*/

void SpotLight::setAmbient(const glm::vec3 ambient)
{
	m_Ambient = ambient * m_Intensity;
	m_LightShape->setAmbient(m_Ambient);
	m_LightingShader->use();
	m_LightingShader->setVec3("spotLights[" + std::to_string(m_SpotLightID) + "].ambient", m_Ambient);

}

void SpotLight::setDiffuse(const glm::vec3 diffuse)
{
	m_Diffuse = diffuse * m_Intensity;
	m_LightShape->setDiffuse(m_Diffuse);
	m_LightingShader->use();
	m_LightingShader->setVec3("spotLights[" + std::to_string(m_SpotLightID) + "].diffuse", m_Diffuse);
}

void SpotLight::setSpecular(const glm::vec3 specular)
{
	m_Specular = specular * m_Intensity;
	m_LightShape->setSpecular(m_Specular);
	m_LightingShader->use();
	m_LightingShader->setVec3("spotLights[" + std::to_string(m_SpotLightID) + "].specular", m_Specular);
}

void SpotLight::setLightDirection(const glm::vec3 newDir)
{
	m_LightDirection = glm::normalize(newDir);
	m_LightingShader->use();
	m_LightingShader->setVec3("spotLights[" + std::to_string(m_SpotLightID) + "].direction", m_LightDirection);
}

void SpotLight::setLightPos(const glm::vec3 lightPos)
{
	m_LightPos = lightPos;
	m_LightingShader->use();
	m_LightingShader->setVec3("spotLights[" + std::to_string(m_SpotLightID) + "].position", m_LightPos);
	if (m_LightShape != nullptr)
		m_LightShape->setPosition(lightPos);
}

void SpotLight::setFarPlane(const float& val)
{
	m_FarPlane = val;
}

void SpotLight::setNearPlane(const float& val)
{
	m_NearPlane = val;
}

void SpotLight::setConstant(const float constant)
{
	m_Constant = constant;
	m_LightingShader->use();
	m_LightingShader->setFloat("spotLights[" + std::to_string(m_SpotLightID) + "].constant", m_Constant);
}

void SpotLight::setLinear(const float linear)
{
	m_Linear = linear;
	m_LightingShader->use();
	m_LightingShader->setFloat("spotLights[" + std::to_string(m_SpotLightID) + "].linear", m_Linear);
}

void SpotLight::setQuadratic(const float quadratic)
{
	m_Quadratic = quadratic;
	m_LightingShader->use();
	m_LightingShader->setFloat("spotLights[" + std::to_string(m_SpotLightID) + "].quadratic", m_Quadratic);
}

void SpotLight::setIntensity(const float i)
{
	m_Intensity = i;
}

/*###################################
* ###     GETTER FUNCTIONS        ###
###################################*/

glm::vec3 SpotLight::getLightPos() const
{
	return m_LightPos;
}

glm::vec3 SpotLight::getLightDirection() const
{
	return 	m_LightDirection;
}

glm::vec3 SpotLight::getAmbient() const
{
	return m_Ambient;
}

glm::vec3 SpotLight::getDiffuse() const
{
	return m_Diffuse;
}

glm::vec3 SpotLight::getSpecular() const
{
	return m_Specular;
}

float SpotLight::getIntensity() const
{
	return m_Intensity;
}

float SpotLight::getNearPlane() const
{
	return m_NearPlane;
}

float SpotLight::getFarPlane() const
{
	return m_FarPlane;
}
