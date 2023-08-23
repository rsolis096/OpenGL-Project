#include "LightSource.h"

LightSource::LightSource()
{
}

LightSource::LightSource(std::vector<float> inputVertices, std::vector<float> inputNormals, const char* type, Shader& lightingShader, Shader& lightCubeShader)
{
	std::cout << "Base class constructor called" << std::endl;
	m_lightingShader = lightingShader;
	m_lightShapeShader = lightCubeShader;
	m_LightPos = glm::vec3(1.2f, 1.0f, 2.0f);
	m_LightShape = new Cube(inputVertices, inputNormals, m_lightShapeShader.ID);

	m_Ambient = glm::vec3(0.2f, 0.2f, 0.2f);
	m_Diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
	m_Specular = glm::vec3(1.0f, 1.0f, 1.0f);

	m_Constant = 1.0f;
	m_Linear = 0.09f;
	m_Quadratic = 0.032f;
}

LightSource::~LightSource()
{
}

void LightSource::initializeLight(glm::vec3 cameraPos)
{
	// light properties
	m_lightingShader.use();
	m_lightingShader.setVec3("light.position", m_LightPos);
	m_lightingShader.setVec3("viewPos", cameraPos);
	m_lightingShader.setVec3("light.ambient", m_Ambient);
	m_lightingShader.setVec3("light.diffuse", m_Diffuse);
	m_lightingShader.setVec3("light.specular", m_Specular);
	m_lightingShader.setFloat("light.constant", m_Constant);
	m_lightingShader.setFloat("light.linear", m_Linear);
	m_lightingShader.setFloat("light.quadratic", m_Quadratic);
	// material properties
	m_lightingShader.setFloat("material.shininess", 64.0f);
}

//For Light Properties
void LightSource::setLightPos(glm::vec3 lightPos)
{
	m_LightPos = lightPos;
	m_lightingShader.use();
	m_lightingShader.setVec3("light.position", m_LightPos);
}

void LightSource::setAmbient(glm::vec3 ambient)
{
	m_Ambient = ambient;
	m_lightingShader.use();
	m_lightingShader.setVec3("light.ambient", m_Ambient);

}

void LightSource::setDiffuse(glm::vec3 diffuse)
{
	m_Diffuse = diffuse;
	m_lightingShader.use();
	m_lightingShader.setVec3("light.diffuse", m_Diffuse);
}

void LightSource::setSpecular(glm::vec3 specular)
{
	m_Specular = specular;
	m_lightingShader.use();
	m_lightingShader.setVec3("light.specular", m_Specular);
}

//For Attenuation
void LightSource::setconstant(float constant)
{
	m_Constant = constant;
	m_lightingShader.use();
	m_lightingShader.setFloat("light.constant", m_Constant);
}

void LightSource::setLinear(float linear)
{
	m_Linear = linear;
	m_lightingShader.use();
	m_lightingShader.setFloat("light.linear", m_Linear);
}

void LightSource::setQuadratic(float quadratic)
{
	m_Quadratic = quadratic;
	m_lightingShader.use();
	m_lightingShader.setFloat("light.quadratic", m_Quadratic);
}

