#pragma once
#include "Camera.h"
#include "Shader.h"
#include "PointLight.h"
#include "DirectionalLight.h"
#include "SpotLight.h"

class LightController
{
public:
	LightController(Shader*, Shader*, Camera*);
	
	//Given by the scene
	Shader* m_LightingShader;
	Shader* m_ObjectShader;
	Camera* m_PlayerCamera;

	DirectionalLight* m_WordLight;
	vector<PointLight*> m_PointLights;
	vector<SpotLight*> m_SpotLights;

	//Point Light Methods
	void addPointLight();
	void removePointLight();

	//Spot Light methods
	void addSpotLight();
	void addSpotLight(glm::vec3&, glm::vec3&);
	void removeSpotLight();

	void drawLighting();
};
