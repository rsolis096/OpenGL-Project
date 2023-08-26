#include "PointLight.h"

int PointLight::pointLightCount = 0;

PointLight::PointLight(Shader& lightingShader, Shader& lightCubeShader, Camera& cam) : playerCamera(cam)//, m_LightModel(Model("resources/objects/lightbulb/lightbulb.obj"))
	{
	lightID = pointLightCount;
	pointLightCount += 1;

	//Builds a light source with some default values
	m_lightingShader = lightingShader; //For changing the lighting properties
	m_lightShapeShader = lightCubeShader; //For rendering the light source itself

	m_LightPos = glm::vec3(0.7f, 0.2f, 2.0f);
	m_LightShape = new Object("Sphere", m_lightShapeShader); //This object will be the lamp

	m_LightShape->setPosition(m_LightPos);

	//For the light bulb location
	m_LightShape->model = glm::mat4(1.0f);
	m_LightShape->model = glm::translate(m_LightShape->model, m_LightShape->m_Position);

	//Light Color Properties
	m_Ambient = glm::vec3(0.05f, 0.05f, 0.05f); //Dark ambient
	m_Diffuse = glm::vec3(0.8f, 0.8f, 0.8f); //Grey light color
	m_Specular = glm::vec3(1.0f, 1.0f, 1.0f); //"Shine" color

	//For attenuation
	m_Constant = 1.0f;
	m_Linear = 0.09f;
	m_Quadratic = 0.032f;

	m_lightShapeShader.use();
	m_lightingShader.setVec3("pointLights[0].position", m_LightPos);
	m_lightingShader.setVec3("pointLights[0].ambient", m_Ambient);
	m_lightingShader.setVec3("pointLights[0].diffuse", m_Diffuse);
	m_lightingShader.setVec3("pointLights[0].specular", m_Specular);
	m_lightingShader.setFloat("pointLights[0].constant", m_Constant);
	m_lightingShader.setFloat("pointLights[0].linear", m_Linear);
	m_lightingShader.setFloat("pointLights[0].quadratic", m_Quadratic);
	m_lightingShader.setFloat("material.shininess", 32.0f);
}

void PointLight::renderLight(glm::mat4 view, glm::mat4 projection)
{

	//MVP
	m_lightShapeShader.use();
	m_lightShapeShader.setMat4("projection", projection);
	m_lightShapeShader.setMat4("view", view);
	m_LightShape->model = glm::mat4(1.0f);
	m_LightShape->model = glm::translate(m_LightShape->model, m_LightPos); //vec is lightPos
	m_LightShape->model = glm::scale(m_LightShape->model, glm::vec3(0.2f)); // a smaller cube
	m_lightShapeShader.setMat4("model", m_LightShape->model);

	//render lamp object
	m_LightShape->render();

	// Light color properties
	m_lightingShader.use();
	m_lightingShader.setVec3("viewPos", playerCamera.cameraPos);
	m_lightingShader.setVec3("pointLights["+std::to_string(lightID)+"].position", m_LightPos);
	m_lightingShader.setVec3("pointLights[" + std::to_string(lightID) + "].ambient", m_Ambient);
	m_lightingShader.setVec3("pointLights[" + std::to_string(lightID) + "].diffuse", m_Diffuse);
	m_lightingShader.setVec3("pointLights[" + std::to_string(lightID) + "].specular", m_Specular);
	m_lightingShader.setFloat("pointLights[" + std::to_string(lightID) + "].constant", m_Constant);
	m_lightingShader.setFloat("pointLights[" + std::to_string(lightID) + "].linear", m_Linear);
	m_lightingShader.setFloat("pointLights[" + std::to_string(lightID) + "].quadratic", m_Quadratic);
	m_lightingShader.setFloat("material.shininess", 32.0f);
}


PointLight::~PointLight()
{
	delete m_LightShape;
	m_LightShape = nullptr;
}

//For Light Properties
void PointLight::setLightPos(glm::vec3 lightPos)
{
	m_LightPos = lightPos;
	m_lightingShader.use();
	m_lightingShader.setVec3("pointlight[" + std::to_string(lightID) + "].position", m_LightPos);
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

//For Attenuation
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



