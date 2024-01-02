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

	Shader* m_LightingShader; //How the light interacts with the world
	Shader* m_ObjectShader; //How the light source itself is drawn
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

	void drawLighting(glm::mat4, glm::mat4);
};
