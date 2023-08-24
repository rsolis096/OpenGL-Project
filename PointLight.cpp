#include "PointLight.h"

PointLight::PointLight(std::vector<float> inputVertices, std::vector<float> inputNormals, const char* type,
	Shader& lightingShader, Shader& lightCubeShader) : LightSource(inputVertices, inputNormals, type,
		lightingShader, lightCubeShader)
{


}

void PointLight::renderLight(glm::vec3 cameraPos, glm::mat4 projection, glm::mat4 view)
{
	//Draw Lamp Object
	m_lightShapeShader.use();
	m_lightingShader.setVec3("viewPos", cameraPos);
	m_lightShapeShader.setMat4("projection", projection);
	m_lightShapeShader.setMat4("view", view);
	m_LightShape->model = glm::mat4(1.0f);
	m_LightShape->model = glm::translate(m_LightShape->model, m_LightPos); //vec is lightPos
	m_LightShape->model = glm::scale(m_LightShape->model, glm::vec3(0.2f)); // a smaller cube
	m_lightShapeShader.setMat4("model", m_LightShape->model);
	m_LightShape->render();
}

void PointLight::initializeLight(glm::vec3 cameraPos, glm::vec3 cameraFront)
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

