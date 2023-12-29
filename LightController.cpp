#include "LightController.h"

LightController::LightController
(Shader& lightingShader, Shader& objectShader, Camera* cam)
	:m_WordLight(new DirectionalLight(lightingShader))
{
	m_LightingShader = lightingShader;
	m_ObjectShader = objectShader;
	m_PlayerCamera = cam;
	glCheckError();
}

void LightController::addPointLight()
{
	m_PointLights.push_back(new PointLight(m_LightingShader, m_ObjectShader, *m_PlayerCamera));
	glCheckError();
}

void LightController::removePointLight()
{
}

void LightController::addSpotLight()
{
	m_SpotLights.push_back(new SpotLight(m_LightingShader, *m_PlayerCamera));
}

void LightController::removeSpotLight()
{
}

void LightController::drawLighting(glm::mat4 view, glm::mat4 projection)
{
	for (PointLight* pointLight : m_PointLights)
		pointLight->renderLight(view, projection);
	for (SpotLight* spotLights : m_SpotLights)
		spotLights->renderLight();

	m_WordLight->renderLight();
}