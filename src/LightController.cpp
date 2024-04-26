#include "LightController.h"
#include "Scene.h"

LightController::LightController
(Shader* lightingShader, Shader* objectShader, Camera* cam, Scene* s)
	:m_WordLight(new DirectionalLight(lightingShader)), m_Scene(s)
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
	m_SpotLights.push_back(new SpotLight(m_LightingShader, m_ObjectShader, *m_PlayerCamera));
	m_Scene->m_ShadowMap->addShadowMap();
}

void LightController::addSpotLight(glm::vec3 pos, glm::vec3 dir)
{
	m_SpotLights.emplace_back(new SpotLight(m_LightingShader, m_ObjectShader, pos, dir));
	m_Scene->m_ShadowMap->addShadowMap();	
	cout << "Added " + (*(m_SpotLights.end() - 1))->m_DisplayName << endl;
}

void LightController::removeSpotLight()
{
}

void LightController::drawLighting()
{
	for (PointLight* pointLight : m_PointLights)
		pointLight->Draw();
	for (SpotLight* spotLights : m_SpotLights)
		spotLights->Draw();

	//m_WordLight->renderLight();
}