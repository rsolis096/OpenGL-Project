#include "LightSource.h"

LightSource::LightSource()
{
}

LightSource::LightSource(std::vector<float> inputVertices, std::vector<float> inputNormals, const char* type, Shader& lightingShader, Shader& lightCubeShader)
{
	//Builds a light source with some default values

	m_lightingShader = lightingShader; //For changing the lighting properties
	m_lightShapeShader = lightCubeShader; //For rendering the light source itself

	m_LightPos = glm::vec3(1.0f, 1.0f, 2.0f);
	m_LightShape = new Cube(inputVertices, inputNormals, m_lightShapeShader);
	m_LightShape->setPosition(m_LightPos);

	m_LightShape->model = glm::mat4(1.0f);
	m_LightShape->model = glm::translate(m_LightShape->model, m_LightShape->m_Position);;

	m_Ambient = glm::vec3(0.2f, 0.2f, 0.2f); //Dark ambient
	m_Diffuse = glm::vec3(0.5f, 0.5f, 0.5f); //Grey light color
	m_Specular = glm::vec3(0.8f, 0.8f, 0.8f); //"Shine" color

	//All lights will have the same initial drop off
	m_Constant = 1.0f;
	m_Linear = 0.09f;
	m_Quadratic = 0.032f;
}

LightSource::~LightSource()
{
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

