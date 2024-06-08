#include "Scene.h"

class PhysicsWorld;

Scene::Scene(Camera* mC)
{
	m_SceneObjectCount = 0;
	fps = 0;
	mainCamera = mC;

	cubeMapShader = new Shader("shaders/skyBoxShader.vert", "shaders/skyBoxShader.frag");
	lightingShader = new Shader("shaders/lightingShader.vert", "shaders/lightingShader.frag");
	//lightingShader = new Shader("shaders/visualizeNormals.vert", "shaders/visualizeNormals.frag", "shaders/visualizeNormals.gs");
	pointLightShader = new Shader("shaders/pointLightShader.vert", "shaders/pointLightShader.frag");

	m_PhysicsWorld = new PhysicsWorld();

	m_LightController = new LightController(lightingShader, pointLightShader, mainCamera, this);
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
	//Draw Point Light Lamp (light spheres)
	pointLightShader->use();
	pointLightShader->setMat4("projection", proj);
	pointLightShader->setMat4("view", view);

	//Set Main Shader matrices
	lightingShader->use();
	lightingShader->setMat4("projection", proj);
	lightingShader->setMat4("view", view);
	lightingShader->setVec3("viewPos", mainCamera->cameraPos);

	//Update shadows only if there exists light matrices (proj matrices from light source)
	if (m_ShadowMap->getLightSpaceMatrices().size() > 0)
	{
		glUniformMatrix4fv(
			glGetUniformLocation(lightingShader->m_ProgramId, "lightSpaceMatrices"),
			m_ShadowMap->getLightSpaceMatrices().size(),
			GL_FALSE,
			glm::value_ptr(m_ShadowMap->getLightSpaceMatrices()[0])
		);
	}

	//Activate the texture units and bind the correspoding depth map
	m_ShadowMap->drawShadowMap(lightingShader->m_ProgramId);

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