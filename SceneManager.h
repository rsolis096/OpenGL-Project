#pragma once
#include "Primitive.h"
#include "Model.h"

class SceneManager
{
public:

	//Total Objects in scene
	int objectCount;

	// Scene Objects (All)
	std::vector<Object*> sceneObjects;

	//Scene Objects (Specific)
	std::vector<Model*> sceneModels;
	std::vector<Primitive*> scenePrimitives;
	//std::vector<LightSource*>& sceneLightSources;

	//Constructors
	SceneManager();

	void addPrimitive(Primitive* obj);
	void addModel(Model* obj);
	void addLightSource();
};