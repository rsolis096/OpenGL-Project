#include "Scene.h"

class PhysicsWorld;

Scene::Scene()
{
	objectCount = 0;
	fps = 0;
	mainCamera = nullptr;
	m_LightController = nullptr;

	cubeMapShader = new Shader("cubeMapShader.vert", "cubeMapShader.frag");
	lightingShader = new Shader("lightingShader.vert", "lightingShader.frag");
	pointLightShader = new Shader("pointLightShader.vert", "pointLightShader.frag");
	
	m_PhysicsWorld = new PhysicsWorld();

	createLightController();

	m_ShadowMap = new ShadowMap(&m_SceneObjects, &m_LightController->m_SpotLights);
	m_SkyBox = nullptr;

	glCheckError();
}

Scene::Scene(Camera* mC)
{
	objectCount = 0;
	fps = 0;
	mainCamera = mC;

	cubeMapShader = new Shader("cubeMapShader.vert", "cubeMapShader.frag");
	lightingShader = new Shader("lightingShader.vert", "lightingShader.frag");
	pointLightShader = new Shader("pointLightShader.vert", "pointLightShader.frag");
	m_PhysicsWorld = new PhysicsWorld();
	createLightController();	

	m_ShadowMap = new ShadowMap(&m_SceneObjects, &m_LightController->m_SpotLights);
	m_SkyBox = new SkyBox(*cubeMapShader, mainCamera);

	glCheckError();
}


//Add an object to the scene (only objects part of a scene are rendered)
int Scene::addObject(Object* obj)
{
	if (obj == nullptr)
		return 1;

	std::vector<Object*>::iterator it = std::find(m_SceneObjects.begin(), m_SceneObjects.end(), obj);
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
	m_LightController = new LightController(lightingShader, pointLightShader, mainCamera, this);
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

void Scene::drawScene(float deltaTime, glm::mat4& proj, glm::mat4& view)
{	
	//Point Light Lamp (light spheres)
	pointLightShader->use();
	pointLightShader->setMat4("projection", proj);
	pointLightShader->setMat4("view", view);

	//Main Shader
	lightingShader->use();
	lightingShader->setMat4("projection", proj);
	lightingShader->setMat4("view", view);
	lightingShader->setVec3("viewPos", mainCamera->cameraPos);

	glUseProgram(lightingShader->ID);
	glUniformMatrix4fv(
		glGetUniformLocation(lightingShader->ID, "lightSpaceMatrices"),
		m_ShadowMap->getLightSpaceMatrices().size(),
		GL_FALSE,
		glm::value_ptr(m_ShadowMap->getLightSpaceMatrices()[0])
	);

	//Activate the texture units and bind the correspoding depth map
	m_ShadowMap->drawShadowMap(lightingShader->ID);

	//Update physics
	m_PhysicsWorld->step(glfwGetTime(), deltaTime);

	//Draw Lights
	m_LightController->drawLighting();

	//Draw Objects
	for (Object* element : m_SceneObjects)
	{
		element->Draw(*lightingShader);
	}

	m_SkyBox->draw(proj);

	//Reset current unit to zero for next render pass
	TextureManager::m_CurrentUnit = 0;
}