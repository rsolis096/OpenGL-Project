#include "PointLight.h"

unsigned short PointLight::m_PointLightCount = 0;
bool firstFrame = true;

//Create Point Light with positon (default = 0,10,0)
PointLight::PointLight(Shader* lightingShader, Shader* objectShader, const glm::vec3& pos) :
	m_LightingShader(lightingShader), m_LightSourceShader(objectShader)
{
	m_LightShape = new Cube(); //Create the physical light object
	setLightPos(pos);
	m_LightShape->setScale(glm::vec3(0.1f, 0.1f, 0.1f));

	m_LightID = m_PointLightCount;
	m_DisplayName = "PointLight" + std::to_string(m_LightID);
	m_PointLightCount++;
	m_LightShape->m_DisplayName = "PointLight" + std::to_string(m_LightID);

	//Light Color Properties (How it casts light on objects)
	m_Ambient = glm::vec3(0.05f, 0.05f, 0.05f); //Dark ambient
	m_Diffuse = glm::vec3(1.0f, 1.0f, 1.0f); //Grey light color
	m_Specular = glm::vec3(0.7f, 0.7f, 0.7f); //"Shine" color

	//Set the color for the light object
	m_LightShape->m_Diffuse = m_Diffuse;
	m_LightShape->m_Ambient = m_Ambient;
	m_LightShape->m_Ambient = m_Ambient;

	//For attenuation
	m_Constant = 1.0f;
	m_Linear = 0.09f;
	m_Quadratic = 0.032f;

	//Set these properties in the lightingShader.frag for the corresponding lightsource at index m_LightID
	m_LightingShader->use();
	m_LightingShader->setInt("numberOfPointLights", m_PointLightCount);
	m_LightingShader->setVec3("pointLights[" + std::to_string(m_LightID) + "].ambient", m_Ambient);
	m_LightingShader->setVec3("pointLights[" + std::to_string(m_LightID) + "].diffuse", m_Diffuse);
	m_LightingShader->setVec3("pointLights[" + std::to_string(m_LightID) + "].specular", m_Specular);
	m_LightingShader->setFloat("pointLights[" + std::to_string(m_LightID) + "].constant", m_Constant);
	m_LightingShader->setFloat("pointLights[" + std::to_string(m_LightID) + "].linear", m_Linear);
	m_LightingShader->setFloat("pointLights[" + std::to_string(m_LightID) + "].quadratic", m_Quadratic);
	m_LightingShader->setVec3("pointLights[" + std::to_string(m_LightID) + "].position", m_LightPos);
}

PointLight::~PointLight()
{
	//TODO: Implement Rule of 3 and 5 if you want
	m_LightingShader = nullptr;
	m_LightSourceShader = nullptr;
	delete m_LightShape;
	m_LightShape = nullptr;
}

void PointLight::Draw()
{
	//Render the light Object
	m_LightShape->Draw(*m_LightSourceShader);
}

/*###################################
* ###     SETTER FUNCTIONS        ###
###################################*/

void PointLight::setLightPos(const glm::vec3 lightPos)
{
	m_LightPos = lightPos;
	m_LightShape->setPosition(lightPos);
	m_LightingShader->use();
	m_LightingShader->setVec3("pointLights[" + std::to_string(m_LightID) + "].position", m_LightPos);
	glCheckError();
}

void PointLight::setAmbient(const glm::vec3 ambient)
{
	m_Ambient = ambient;
	m_LightShape->m_Ambient = ambient;
	m_LightingShader->use();
	m_LightingShader->setVec3("pointLights[" + std::to_string(m_LightID) + "].ambient", m_Ambient);
}

void PointLight::setDiffuse(const glm::vec3 diffuse)
{
	m_Diffuse = diffuse;	
	m_LightShape->m_Diffuse = diffuse;
	m_LightingShader->use();
	m_LightingShader->setVec3("pointLights[" + std::to_string(m_LightID) + "].diffuse", m_Diffuse);
}

void PointLight::setSpecular(const glm::vec3 specular)
{
	m_Specular = specular;
	m_LightShape->m_Specular = specular;
	m_LightingShader->use();
	m_LightingShader->setVec3("pointLights[" + std::to_string(m_LightID) + "].specular", m_Specular);
}

void PointLight::setconstant(const float constant)
{
	m_Constant = constant;
	m_LightingShader->use();
	m_LightingShader->setFloat("pointLights[" + std::to_string(m_LightID) + "].constant", m_Constant);
}

void PointLight::setLinear(const float linear)
{
	m_Linear = linear;
	m_LightingShader->use();
	m_LightingShader->setFloat("pointLight[" + std::to_string(m_LightID) + "].linear", m_Linear);
}

void PointLight::setQuadratic(const float quadratic)
{
	m_Quadratic = quadratic;
	m_LightingShader->use();
	m_LightingShader->setFloat("pointLights[" + std::to_string(m_LightID) + "].quadratic", m_Quadratic);
}

/*###################################
* ###     GETTER FUNCTIONS        ###
###################################*/

glm::vec3 PointLight::getLightPos()
{
	return m_LightPos;
}

glm::vec3 PointLight::getAmbient()
{
	return m_Ambient;
}

glm::vec3 PointLight::getDiffuse()
{
	return m_Diffuse;
}

glm::vec3 PointLight::getSpecular()
{
	return m_Specular;
}