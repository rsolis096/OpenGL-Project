#include "PointLight.h"

PointLight::PointLight(std::vector<float> inputVertices, std::vector<float> inputNormals, const char* type,
	Shader& lightingShader, Shader& lightCubeShader) : LightSource(inputVertices, inputNormals, type,
		lightingShader, lightCubeShader)
{
	std::cout << "Derived class constructor called" << std::endl;


}

void PointLight::renderLight(glm::mat4 projection, glm::mat4 view)
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

PointLight::~PointLight()
{
}

