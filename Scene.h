#pragma once

#include "Model.h"
#include "Camera.h"
#include "LightController.h"
#include "PhysicsWorld.h"
#include "ShadowMap.h"
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

	void drawScene(float deltaTime);
	void drawScene(float deltaTime, Shader& shader);
};