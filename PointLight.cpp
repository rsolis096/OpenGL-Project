#include "PointLight.h"

PointLight::PointLight(std::vector<float> inputVertices, std::vector<float> inputNormals,
	Shader& lightingShader, Shader& lightCubeShader, Camera& cam) : LightSource(inputVertices, inputNormals,
		lightingShader, lightCubeShader, cam)
{
	m_Ambient = glm::vec3(0.2f, 0.2f, 0.2f); //Dark ambient
	m_Diffuse = glm::vec3(0.5f, 0.5f, 0.5f); //Grey light color
	m_Specular = glm::vec3(1.0f, 1.0f, 1.0f); //"Shine" color
}

void PointLight::renderLight(glm::mat4 view, glm::mat4 projection)
{
	//Draw Lamp Object
	initializeLight();
	m_lightShapeShader.use();
	m_lightingShader.setVec3("viewPos", playerCamera.cameraPos);
	m_lightShapeShader.setMat4("projection", projection);
	m_lightShapeShader.setMat4("view", view);
	m_LightShape->model = glm::mat4(1.0f);
	m_LightShape->model = glm::translate(m_LightShape->model, m_LightPos); //vec is lightPos
	m_LightShape->model = glm::scale(m_LightShape->model, glm::vec3(0.2f)); // a smaller cube
	m_lightShapeShader.setMat4("model", m_LightShape->model);
	m_LightShape->render();
}

void PointLight::initializeLight()
{
	//Default Light Properties
	m_lightingShader.use();
	//Light Type (1 for point light)
	m_lightingShader.setInt("lightFlag", 1);

	//
	m_lightingShader.setVec3("light.position", m_LightPos);
	//m_lightingShader.setVec3("viewPos", cameraPos); //Constantly changing, update every frame
	m_lightingShader.setVec3("light.ambient", m_Ambient);
	m_lightingShader.setVec3("light.diffuse", m_Diffuse);
	m_lightingShader.setVec3("light.specular", m_Specular);
	m_lightingShader.setFloat("light.constant", m_Constant);
	m_lightingShader.setFloat("light.linear", m_Linear);
	m_lightingShader.setFloat("light.quadratic", m_Quadratic);
	// material properties
	m_lightingShader.setFloat("material.shininess", 64.0f);
}

PointLight::~PointLight()
{
}

