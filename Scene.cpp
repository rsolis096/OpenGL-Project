#include "Scene.h"

Scene::Scene()
{
	objectCount = 0;
	worldTime = 0;
	fps = 0;
	mainCamera = nullptr;
}

void Scene::addObject(Object* obj)
{
	if (obj != nullptr)
	{
		sceneObjects.push_back(obj);
		objectCount++;
	}
}


void Scene::addLightSource()
{
}
