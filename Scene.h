#pragma once

#include "Model.h"

class Scene
{
public:

	//Total Objects in scene
	int objectCount;

	// Scene Objects (All)
	std::vector<Object*> sceneObjects;

	//Scene Objects (Specific)
	std::vector<Model*> sceneModels;

	//Constructors
	Scene();
	void addObject(Object* obj);
	void addLightSource();
};