#include "LightController.h"
#include "Scene.h"

LightController::LightController
(Shader* lightingShader, Shader* objectShader, Camera* cam, Scene* s): m_Scene(s)
{
	m_LightingShader = lightingShader;
	m_LightSourceShader = objectShader;
	m_DirectionalLight = nullptr;
	m_PlayerCamera = cam;
	glCheckError();
}



void LightController::addPointLight(const glm::vec3& pos)
{
	m_PointLights.push_back(new PointLight(m_LightingShader, m_LightSourceShader, pos));
	PointLight* newPointLight = m_PointLights.back();
	m_Scene->m_ShadowMap->addPointLightShadowMap(newPointLight->getCubeMapTexture());
	glCheckError();
}

void LightController::removePointLight()
{
	//TODO:  LightController::removePointLight()
}

void LightController::addSpotLight(const glm::vec3 pos, const glm::vec3 dir)
{
	m_SpotLights.push_back(new SpotLight(m_LightingShader, m_LightSourceShader, pos, dir));
	m_Scene->m_ShadowMap->addSpotLightShadowMap(m_SpotLights[m_SpotLights.size() - 1]->getDepthMapTexture());
	glCheckError();
}

void LightController::removeSpotLight()
{
	//TODO:  LightController::removeSpotLight()
}

void LightController::addDirectionalLight(const glm::vec3& dir)
{
	//Only one directional light can exist
	if(m_DirectionalLight != nullptr){
		delete m_DirectionalLight;
		m_DirectionalLight = nullptr;
	}
	m_DirectionalLight = new DirectionalLight(m_LightingShader, dir);

	// Configure a directional shadow map
	m_Scene->m_ShadowMap->addDirectionalShadowMap(m_DirectionalLight->getDepthMapTexture());
	glCheckError();
}

void LightController::removeDirectionalLight()
{
	if (m_DirectionalLight != nullptr) {
		delete m_DirectionalLight;
		m_DirectionalLight = nullptr;
	}
}

void LightController::drawLighting() const
{
	for (PointLight* pointLight : m_PointLights) {
		pointLight->Draw();
	}
	for (SpotLight* spotLights : m_SpotLights) {
		spotLights->Draw();
	}
	//m_WordLight->renderLight();
}