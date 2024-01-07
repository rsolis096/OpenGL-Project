#pragma once

#include "PhysicsWorld.h"
#include "Model.h"
#include "Camera.h"
#include "LightController.h"
#include "ShadowMap.h"
#include "SkyBox.h"
#include <typeinfo>

class Scene
{
public:

	//Total Objects in scene
	int objectCount;
	float fps;

	Camera* mainCamera;

	LightController* m_LightController;
	PhysicsWorld* m_PhysicsWorld;

	Shader* cubeMapShader;
	Shader* lightingShader;
	Shader* pointLightShader;

	ShadowMap* m_ShadowMap;

	SkyBox* m_SkyBox;

	// Scene Objects (All)
	std::vector<Object*> m_SceneObjects;

	//Scene Objects (Specific)
	std::vector<Model*> sceneModels;

	//Constructors
	Scene();
	Scene(Camera*);
	int addObject(Object* obj);
	int removeObject(Object* obj);
	void removeAllObjects();

	void addLightController(LightController* lc);
	void createLightController();
	void removeLightController();

	//void addShader(Shader&);

	void drawScene(float, glm::mat4&);
	void drawScene(float, Shader&, glm::mat4&);
};