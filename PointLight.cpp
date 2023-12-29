#include "PointLight.h"

int PointLight::pointLightCount = 0;
bool firstFrame = true;

PointLight::PointLight(Shader& lightingShader, Shader& objectShader, Camera& cam) : playerCamera(cam),
m_lightingShader(lightingShader), m_ObjectShader(objectShader)
{
	glCheckError();

	m_LightShape = new Cube(); //Create the physical light
	glCheckError();

	lightID = pointLightCount;
	glCheckError();

	m_LightShape->m_DisplayName = "PointLight" + std::to_string(lightID);
	glCheckError();

	pointLightCount++;


	//Light and its corresponding object starts at center of world
	setLightPos(glm::vec3(0.0f, 0.0f, 0.0f));
	glCheckError();

	m_LightShape->setScale(glm::vec3(0.1f, 0.1f, 0.1f));
	glCheckError();


	//Builds a light source with some default values
	//m_lightingShader = lightingShader; //For changing the lighting properties
	//m_ObjectShader = objectShader; //For rendering the light object itself (a cube)

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

	//Set these properties in the lightingShader.frag, this is used when rendering all objects
	//These do not need to change every draw call
	glCheckError();

	m_lightingShader.use();
	glCheckError();

	m_lightingShader.setFloat("material.shininess", 128.0f);
	glCheckError();

	//These can be changed in a change color function to change the way the light color itself
	m_lightingShader.setVec3("pointLights[" + std::to_string(lightID) + "].ambient", m_Ambient);
	glCheckError();

	m_lightingShader.setVec3("pointLights[" + std::to_string(lightID) + "].diffuse", m_Diffuse);
	glCheckError();

	m_lightingShader.setVec3("pointLights[" + std::to_string(lightID) + "].specular", m_Specular);
	glCheckError();


	m_lightingShader.setFloat("pointLights[" + std::to_string(lightID) + "].constant", m_Constant);
	glCheckError();

	m_lightingShader.setFloat("pointLights[" + std::to_string(lightID) + "].linear", m_Linear);
	glCheckError();

	m_lightingShader.setFloat("pointLights[" + std::to_string(lightID) + "].quadratic", m_Quadratic);
	glCheckError();

	m_lightingShader.setInt("numberOfPointLights", pointLightCount);
	glCheckError();

	m_lightingShader.setVec3("pointLights[" + std::to_string(lightID) + "].position", m_LightPos);
	GLint positionLocation = glGetUniformLocation(m_lightingShader.ID, ("pointLights[" + std::to_string(lightID) + "].position").c_str());
	if (positionLocation == -1) {
		std::cout << "1.Uniform location not found!" << std::endl;
	}

}

void PointLight::renderLight(glm::mat4 view, glm::mat4 projection)
{

	//Render the light Object
	m_ObjectShader.use();
	m_ObjectShader.setMat4("projection", projection);
	m_ObjectShader.setMat4("view", view);
	m_LightShape->Draw(m_ObjectShader);

	// Set the light properties to be 
	m_lightingShader.use();
	m_lightingShader.setVec3("viewPos", playerCamera.cameraPos);
}


PointLight::~PointLight()
{
	delete m_LightShape;
	m_LightShape = nullptr;
}

void PointLight::setLightPos(glm::vec3 lightPos)
{
	m_LightPos = lightPos;
	glCheckError();

	m_LightShape->setPosition(lightPos);
	glCheckError();

	m_lightingShader.use();
	glCheckError();

	m_lightingShader.setVec3("pointLights[" + std::to_string(lightID) + "].position", m_LightPos);

	GLint positionLocation = glGetUniformLocation(m_lightingShader.ID, ("pointLights[" + std::to_string(lightID) + "].position").c_str());
	if (positionLocation == -1) {
		std::cout << "2.Uniform location not found!" << std::endl;
	}
	glCheckError();
}

void PointLight::setAmbient(glm::vec3 ambient)
{
	m_Ambient = ambient;
	m_lightingShader.use();
	m_lightingShader.setVec3("pointlight[" + std::to_string(lightID) + "].ambient", m_Ambient);
}

void PointLight::setDiffuse(glm::vec3 diffuse)
{
	m_Diffuse = diffuse;
	m_lightingShader.use();
	m_lightingShader.setVec3("pointlight[" + std::to_string(lightID) + "].diffuse", m_Diffuse);
}

void PointLight::setSpecular(glm::vec3 specular)
{
	m_Specular = specular;
	m_lightingShader.use();
	m_lightingShader.setVec3("pointlight[" + std::to_string(lightID) + "].specular", m_Specular);
}

void PointLight::setconstant(float constant)
{
	m_Constant = constant;
	m_lightingShader.use();
	m_lightingShader.setFloat("pointlight[" + std::to_string(lightID) + "].constant", m_Constant);
}

void PointLight::setLinear(float linear)
{
	m_Linear = linear;
	m_lightingShader.use();
	m_lightingShader.setFloat("pointlight[" + std::to_string(lightID) + "].linear", m_Linear);
}

void PointLight::setQuadratic(float quadratic)
{
	m_Quadratic = quadratic;
	m_lightingShader.use();
	m_lightingShader.setFloat("pointlight[" + std::to_string(lightID) + "].quadratic", m_Quadratic);
}



