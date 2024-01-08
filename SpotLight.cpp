#include "SpotLight.h"

bool firstFrame2 = true;
int SpotLight::spotLightCount = 0;


SpotLight::SpotLight(Shader* lightingShader, Shader* objectShader, Camera& cam) :
	m_lightingShader(lightingShader), m_ObjectShader(objectShader), m_LightPos(cam.cameraPos), m_lightDirection(cam.cameraFront), 
	playerCamera(true), m_NearPlane(0.5f), m_FarPlane(70.0f)
{
	spotLightID = spotLightCount;
	spotLightCount++;
	m_DisplayName = "SpotLight" + std::to_string(spotLightID);

	//Object is a flashlight, no need for a sphere to follow the light source
	m_LightShape = nullptr;

	//Light color properties
	m_Intensity = 1;
	m_Ambient = glm::vec3(0.0f, 0.0f, 0.0f);
	m_Diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
	m_Specular = glm::vec3(1.0f, 1.0f, 1.0f);

	//For Attenuation (defaults)
	m_Constant = 1.0f;
	m_Linear = 0.09f;
	m_Quadratic = 0.032f;
	m_Intensity = 1;

	m_lightingShader->use();
	m_lightingShader->setInt("numberOfSpotLightsFRAG", spotLightCount);
	m_lightingShader->setInt("numberOfSpotLightsVERT", spotLightCount);
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

SpotLight::SpotLight(Shader* lightingShader, Shader* objectShader, glm::vec3 pos, glm::vec3 dir) :
	m_lightingShader(lightingShader), m_ObjectShader(objectShader), m_LightPos(pos), m_lightDirection(dir), playerCamera(false),
	m_NearPlane(0.5f), m_FarPlane(70.0f)
{
	m_lightDirection = glm::normalize(dir - pos);

	spotLightID = spotLightCount;
	m_DisplayName = "SpotLight" + std::to_string(spotLightID);
	spotLightCount++;
	//Spotlight takes the shape of a sphere
	m_LightShape = new Sphere();
	m_LightShape->setPosition(pos);
	m_LightShape->setScale(glm::vec3(0.2f));

	//Light color properties
	m_Intensity = 1;
	m_Ambient = glm::vec3(1.0f, 1.0f, 1.0f);
	m_Diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
	m_Specular = glm::vec3(1.0f, 1.0f, 1.0f);

	//For Attenuation
	m_Constant = 1.0f;
	m_Linear = 0.09f;
	m_Quadratic = 0.032f;
	
	std::cout << spotLightCount << std::endl;

	m_lightingShader->use();
	m_lightingShader->setInt("numberOfSpotLightsVERT", spotLightCount);
	m_lightingShader->setInt("numberOfSpotLightsFRAG", spotLightCount);



	// Get the location of the uniform in the shader program
	int numberOfShadowMapsLocation1 = glGetUniformLocation(m_lightingShader->ID, "numberOfShadowMapsFRAG");

	// Set the value of the uniform
	glUseProgram(m_lightingShader->ID);
	glUniform1i(numberOfShadowMapsLocation1, spotLightCount);

	// Get the location of the uniform in the shader program
	int numberOfShadowMapsLocation2 = glGetUniformLocation(m_lightingShader->ID, "numberOfShadowMapsVERT");

	// Set the value of the uniform
	glUseProgram(m_lightingShader->ID);
	glUniform1i(numberOfShadowMapsLocation2, spotLightCount);

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
	if(playerCamera)
	{	
		//These values change every frame since light is attached to camera
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

glm::vec3 SpotLight::getLightPos()
{
	return m_LightPos;
}

void SpotLight::setLightDirection(glm::vec3 newDir)
{
	m_lightDirection = glm::normalize(newDir);
	m_lightingShader->use();
	m_lightingShader->setVec3("spotLights[" + std::to_string(spotLightID) + "].direction", m_lightDirection);
}

glm::vec3 SpotLight::getLightDirection()
{
	return 	m_lightDirection;
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

float SpotLight::getNearPlane()
{
	return m_NearPlane;
}

void SpotLight::setNearPlane(float& val)
{
	m_NearPlane = val;
}

float SpotLight::getFarPlane()
{
	return m_FarPlane;
}

void SpotLight::setFarPlane(float& val)
{
	m_FarPlane = val;
}
