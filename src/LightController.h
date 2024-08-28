#pragma once

#include "PointLight.h"
#include "DirectionalLight.h"
#include "SpotLight.h"

//Forward Declaration (both depend on eachother)
class Scene;

class LightController
{
public:

	enum LightType {
		POINT_LIGHT,       // 0
		SPOT_LIGHT,        // 1
		DIRECTIONAL_LIGHT  // 2
	};

	LightController(Shader*, Shader*, Camera*, Scene*);
	
	//Given by the scene
	Shader* m_LightingShader;
	Shader* m_LightSourceShader;
	Camera* m_PlayerCamera;
	Scene* m_Scene;

	DirectionalLight* m_DirectionalLight;
	std::vector<PointLight*> m_PointLights;
	std::vector<SpotLight*> m_SpotLights;

	//Point Light Methods
	void addPointLight(const glm::vec3&);
	void removePointLight();

	//SpotLight methods
	void addSpotLight(const glm::vec3, const glm::vec3);
	void removeSpotLight();

	//SpotLight methods
	void addDirectionalLight(const glm::vec3&);
	void removeDirectionalLight();

	void drawLighting() const;
};
