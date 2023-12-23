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
	std::vector<Object*> sceneObjects;

	//Scene Objects (Specific)
	std::vector<Model*> sceneModels;

	//Constructors
	Scene();
	void addObject(Object* obj);
	void addLightSource();
};