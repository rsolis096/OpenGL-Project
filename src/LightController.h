#pragma once

#include "PointLight.h"
#include "DirectionalLight.h"
#include "SpotLight.h"

//Forward Declaration (both depend on eachother)
class Scene;

enum LightType {
	SPOT_LIGHT,        // 0
	POINT_LIGHT,       // 1
	DIRECTIONAL_LIGHT  // 2
};

class LightController
{
public:
	LightController(Shader*, Shader*, Camera*, Scene*);
	
	//Given by the scene
	Shader* m_LightingShader;
	Shader* m_LightSourceShader;
	Camera* m_PlayerCamera;
	Scene* m_Scene;

	DirectionalLight* m_WordLight;
	std::vector<PointLight*> m_PointLights;
	std::vector<SpotLight*> m_SpotLights;

	//Point Light Methods
	void addPointLight(const glm::vec3&);
	void removePointLight();

	//Spot Light methods
	void addSpotLight(glm::vec3, glm::vec3);
	void removeSpotLight();

	void drawLighting();
};
