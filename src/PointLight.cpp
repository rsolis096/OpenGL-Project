#include "PointLight.h"

int PointLight::pointLightCount = 0;
bool firstFrame = true;

PointLight::PointLight(Shader* lightingShader, Shader* objectShader, Camera& cam) :
m_lightingShader(lightingShader), m_ObjectShader(objectShader)
{
	m_LightShape = new Cube(); //Create the physical light object
	setLightPos(glm::vec3(0.0f, 10.0f, 0.0f));
	m_LightShape->setScale(glm::vec3(0.1f, 0.1f, 0.1f));

	lightID = pointLightCount;
	pointLightCount++;
	m_LightShape->m_DisplayName = "PointLight" + std::to_string(lightID);

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

	//Set these properties in the lightingShader.frag for the corresponding lightsource at index lightID
	m_lightingShader->use();
	m_lightingShader->setInt("numberOfPointLights", pointLightCount);
	m_lightingShader->setVec3("pointLights[" + std::to_string(lightID) + "].ambient", m_Ambient);
	m_lightingShader->setVec3("pointLights[" + std::to_string(lightID) + "].diffuse", m_Diffuse);
	m_lightingShader->setVec3("pointLights[" + std::to_string(lightID) + "].specular", m_Specular);
	m_lightingShader->setFloat("pointLights[" + std::to_string(lightID) + "].constant", m_Constant);
	m_lightingShader->setFloat("pointLights[" + std::to_string(lightID) + "].linear", m_Linear);
	m_lightingShader->setFloat("pointLights[" + std::to_string(lightID) + "].quadratic", m_Quadratic);
	m_lightingShader->setVec3("pointLights[" + std::to_string(lightID) + "].position", m_LightPos);
}

void PointLight::Draw()
{
	//Render the light Object
	m_LightShape->Draw(*m_ObjectShader);
}

PointLight::~PointLight()
{
	delete m_LightShape;
	m_LightShape = nullptr;
}

void PointLight::setLightPos(glm::vec3 lightPos)
{
	m_LightPos = lightPos;
	m_LightShape->setPosition(lightPos);
	m_lightingShader->use();
	m_lightingShader->setVec3("pointLights[" + std::to_string(lightID) + "].position", m_LightPos);
	glCheckError();
}

void PointLight::setAmbient(glm::vec3 ambient)
{
	m_Ambient = ambient;
	m_LightShape->m_Ambient = ambient;
	m_lightingShader->use();
	m_lightingShader->setVec3("pointlight[" + std::to_string(lightID) + "].ambient", m_Ambient);
}

void PointLight::setDiffuse(glm::vec3 diffuse)
{
	m_Diffuse = diffuse;	
	m_LightShape->m_Diffuse = diffuse;
	m_lightingShader->use();
	m_lightingShader->setVec3("pointlight[" + std::to_string(lightID) + "].diffuse", m_Diffuse);
}

void PointLight::setSpecular(glm::vec3 specular)
{
	m_Specular = specular;
	m_LightShape->m_Specular = specular;
	m_lightingShader->use();
	m_lightingShader->setVec3("pointlight[" + std::to_string(lightID) + "].specular", m_Specular);
}

void PointLight::setconstant(float constant)
{
	m_Constant = constant;
	m_lightingShader->use();
	m_lightingShader->setFloat("pointlight[" + std::to_string(lightID) + "].constant", m_Constant);
}

void PointLight::setLinear(float linear)
{
	m_Linear = linear;
	m_lightingShader->use();
	m_lightingShader->setFloat("pointlight[" + std::to_string(lightID) + "].linear", m_Linear);
}

void PointLight::setQuadratic(float quadratic)
{
	m_Quadratic = quadratic;
	m_lightingShader->use();
	m_lightingShader->setFloat("pointlight[" + std::to_string(lightID) + "].quadratic", m_Quadratic);
}



