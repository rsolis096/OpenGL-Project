#pragma once

#include "Model.h"
#include "Camera.h"

class Scene
{
public:

	//Total Objects in scene
	int objectCount;
	float worldTime;
	float fps;
	Camera* mainCamera;

	// Scene Objects (All)
	std::vector<Object*> m_SceneObjects;

	//Scene Objects (Specific)
	std::vector<Model*> sceneModels;

	//Constructors
	Scene();
	int addObject(Object* obj);
	int removeObject(Object* obj);
	void removeAllObjects();
	void addLightSource();
};