#include "Scene.h"

Scene::Scene()
{
	objectCount = 0;
	fps = 0;
	mainCamera = nullptr;
	m_LightController = nullptr;

	//Compile and link shaders
	cubeMapShader = new Shader("cubeMapShader.vert", "cubeMapShader.frag");
	glCheckError();

	//General Rasterized lighting
	lightingShader = new Shader("lightingShader.vert", "lightingShader.frag");
	glCheckError();

	//For objects that are also light sources
	pointLightShader = new Shader("pointLightShader.vert", "pointLightShader.frag");
	glCheckError();


	m_PhysicsWorld = new PhysicsWorld();

	glCheckError();

}

Scene::Scene(Camera* mC)
{
	objectCount = 0;
	fps = 0;
	mainCamera = mC;
	m_LightController = nullptr;

	//Compile and link shaders
	cubeMapShader = new Shader("cubeMapShader.vert", "cubeMapShader.frag");
	//General Rasterized lighting
	glCheckError();
	lightingShader = new Shader("lightingShader.vert", "lightingShader.frag");
	//For objects that are also light sources
	glCheckError();
	pointLightShader = new Shader("pointLightShader.vert", "pointLightShader.frag");
	glCheckError();
	m_PhysicsWorld = new PhysicsWorld();
	glCheckError();
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

//Create a LightController within the scene
void Scene::createLightController()
{
	if (m_LightController != nullptr)
		removeLightController();
	m_LightController = new LightController(lightingShader, pointLightShader, mainCamera);;
}

//Add a LightController to the scene
void Scene::addLightController(LightController* lc)
{
	if (m_LightController != nullptr)
		removeLightController();
	m_LightController = lc;
}

void Scene::removeLightController()
{

}

void Scene::drawScene(glm::mat4 projection, float deltaTime)
{
	//Update Scene Objects
	for (auto& element : m_SceneObjects)
	{
		m_PhysicsWorld->step(glfwGetTime(), deltaTime);
		if (typeid(element).name()[0] == 'S')
		{
			element->translatePosition(glm::vec3(0.001f));
		}
		if (typeid(element).name()[0] == 'M')
		{
			element->Draw(*lightingShader);
		}
		if (typeid(element).name()[0] != 'M')
			element->Draw(*lightingShader);
	}

	//Draw Lights
	m_LightController->drawLighting(mainCamera->GetViewMatrix(), projection);
}

void Scene::drawScene(glm::mat4 projection, float deltaTime, Shader& shader)
{
	//Update Scene Objects
	for (auto& element : m_SceneObjects)
	{
		//m_PhysicsWorld->step(glfwGetTime(), deltaTime);
		if (typeid(element).name()[0] == 'S')
		{
			element->translatePosition(glm::vec3(0.001f));
		}
		if (typeid(element).name()[0] == 'M')
		{
			element->Draw(shader);
		}
		if (typeid(element).name()[0] != 'M')
			element->Draw(shader);
	}

	//Draw Lights
	m_LightController->drawLighting(mainCamera->GetViewMatrix(), projection);
}

