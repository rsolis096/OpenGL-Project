#include "Scene.h"

Scene::Scene()
{
	objectCount = 0;
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
