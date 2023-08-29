#include "SceneManager.h"

SceneManager::SceneManager()
{
	objectCount = 0;
}

void SceneManager::addPrimitive(Primitive* obj)
{
	if (obj != nullptr)
	{
		scenePrimitives.push_back(obj);
		sceneObjects.push_back(obj);
		objectCount++;
	}
}

void SceneManager::addModel(Model* obj)
{
	if (obj != nullptr)
	{
		sceneModels.push_back(obj);
		sceneObjects.push_back(obj);
		objectCount++;
	}
}

void SceneManager::addLightSource()
{
}
