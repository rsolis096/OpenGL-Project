#include "SpotLight.h"

SpotLight::SpotLight(std::vector<float> inputVertices, std::vector<float> inputNormals, const char* type,
	Shader& lightingShader, Shader& lightCubeShader) : LightSource(inputVertices, inputNormals, type,
		lightingShader, lightCubeShader)
{
	std::cout << "Derived class constructor called" << std::endl;


}

void SpotLight::renderLight(glm::vec3 cameraPos, glm::mat4 projection, glm::mat4 view)
{
	//Draw Lamp Object
	m_lightShapeShader.use();
	m_lightShapeShader.setMat4("projection", projection);
	m_lightShapeShader.setMat4("view", view);
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(1.2f, 1.0f, 2.0f)); //vec is lightPos
	model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
	m_lightShapeShader.setMat4("model", model);
	m_LightShape->render();
}

SpotLight::~SpotLight()
{
}

void SpotLight::initializeLight(glm::vec3 cameraPos, glm::vec3 cameraFront)
{
	//Default Light Properties for spot light
	m_lightingShader.use();
	m_lightingShader.setVec3("light.position", cameraPos);
	m_lightingShader.setVec3("viewPos", cameraPos);
	m_lightingShader.setVec3("light.direction", cameraFront);
	m_lightingShader.setFloat("light.cutOff", glm::cos(glm::radians(12.5f)));

	m_lightingShader.setVec3("light.ambient", m_Ambient);
	m_lightingShader.setVec3("light.diffuse", m_Diffuse);
	m_lightingShader.setVec3("light.specular", m_Specular);
	m_lightingShader.setFloat("light.constant", m_Constant);
	m_lightingShader.setFloat("light.linear", m_Linear);
	m_lightingShader.setFloat("light.quadratic", m_Quadratic);
	m_lightingShader.setInt("lightFlag", 2);
	// material properties
	m_lightingShader.setFloat("material.shininess", 32.0f);
}

