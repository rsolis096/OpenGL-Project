#include "Scene.h"

Scene::Scene()
{
	objectCount = 0;
	worldTime = 0;
	fps = 0;
	mainCamera = nullptr;
}

//Add an object to the scene (only objects part of a scene are rendered)
int Scene::addObject(Object* obj)
{
	std::vector<Object*>::iterator it = std::find(m_SceneObjects.begin(), m_SceneObjects.end(), obj);
	if (obj == nullptr)
		return 1;
	if (it == m_SceneObjects.end())
	{
		m_SceneObjects.push_back(obj);
		return 0;
	}
	std::cout << "Object already in scene!" << std::endl;
	return 1;
}

//Remove an object from the scene
int Scene::removeObject(Object* obj)
{
	//Remove all instances of obj
	std::vector<Object*>::iterator removeIterator = std::remove(m_SceneObjects.begin(), m_SceneObjects.end(), obj);
	if (removeIterator == m_SceneObjects.end())
	{
		std::cout << "Scene: " << obj->m_ObjectID << " is not part of the scene!" << std::endl;
		return 1;
	}
	else
	{
		m_SceneObjects.erase(removeIterator, m_SceneObjects.end());
		std::cout << "Scene: Removed " << obj->m_ObjectID << std::endl;
		return 0;
	}
}

//Remove all objects from the scene
void Scene::removeAllObjects()
{
	for (auto it = m_SceneObjects.begin(); it != m_SceneObjects.end(); ++it)
		delete* it;
	m_SceneObjects.clear();
}

//Add a light to the scene
void Scene::addLightSource()
{
}
