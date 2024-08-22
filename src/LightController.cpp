#include "LightController.h"
#include "Scene.h"

LightController::LightController
(Shader* lightingShader, Shader* objectShader, Camera* cam, Scene* s): m_Scene(s)
{
	m_LightingShader = lightingShader;
	m_LightSourceShader = objectShader;
	m_PlayerCamera = cam;
	glCheckError();
}



void LightController::addPointLight(const glm::vec3& pos)
{
	m_PointLights.push_back(new PointLight(m_LightingShader, m_LightSourceShader, pos));
	m_Scene->m_ShadowMap->addCubeMap();
	glCheckError();
}

void LightController::removePointLight()
{
	//TODO:  LightController::removePointLight()
}

void LightController::addSpotLight(const glm::vec3 pos, const glm::vec3 dir)
{
	m_SpotLights.push_back(new SpotLight(m_LightingShader, m_LightSourceShader, pos, dir));
	m_Scene->m_ShadowMap->addShadowMap();

}

void LightController::removeSpotLight()
{
	//TODO:  LightController::removeSpotLight()
}




void LightController::addDirectionalLight(const glm::vec3& pos)
{

}

void LightController::removeDirectionalLight()
{
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