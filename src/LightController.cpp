#include "LightController.h"
#include "Scene.h"

LightController::LightController
(Shader* lightingShader, Shader* objectShader, Camera* cam, Scene* s)
	:m_WordLight(new DirectionalLight(lightingShader)), m_Scene(s)
{
	m_LightingShader = lightingShader;
	m_LightSourceShader = objectShader;
	m_PlayerCamera = cam;
	glCheckError();
}

void LightController::addPointLight(const glm::vec3& pos)
{
	m_PointLights.push_back(new PointLight(m_LightingShader, m_LightSourceShader, pos));
	glCheckError();
}

void LightController::removePointLight()
{
	//TODO:  LightController::removePointLight()
}

void LightController::addSpotLight(glm::vec3 pos, glm::vec3 dir)
{
	m_SpotLights.push_back(new SpotLight(m_LightingShader, m_LightSourceShader, pos, dir));
	m_Scene->m_ShadowMap->addShadowMap();

}

void LightController::removeSpotLight()
{
	//TODO:  LightController::removeSpotLight()
}

void LightController::drawLighting()
{
	for (PointLight* pointLight : m_PointLights) {
		pointLight->Draw();
	}
	for (SpotLight* spotLights : m_SpotLights) {
		spotLights->Draw();
	}
	//m_WordLight->renderLight();
}