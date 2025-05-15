#include "Scene.h"

//Static members
unsigned int Scene::SCREEN_HEIGHT = 1080;
unsigned int Scene::SCREEN_WIDTH = 1920;
//Forward Declarations
class PhysicsWorld;

Scene::Scene(Camera* mC)
{
	m_SceneObjectCount = 0;
	fps = 0;
	mainCamera = mC;

	cubeMapShader = new Shader("shaders/skyBoxShader.vert", "shaders/skyBoxShader.frag");

	//Enable this to use the main shader
	lightingShader = new Shader("shaders/lightingShader.vert", "shaders/lightingShader.frag");

	//Enable this to view normals
	//lightingShader = new Shader("shaders/visualizeNormals.vert", "shaders/visualizeNormals.frag", "shaders/visualizeNormals.gs");

	//Enable this to render purely point light shadows (for implementing shadows to main shader)
	//lightingShader = new Shader("shaders/testShader.vs", "shaders/testShader.fs");

	pointLightShader = new Shader("shaders/pointLightShader.vert", "shaders/pointLightShader.frag");
	m_PhysicsWorld = new PhysicsWorld();

	m_LightController = new LightController(lightingShader, pointLightShader, mainCamera, this);
	m_ShadowMap = new ShadowMap(&m_SceneObjects, m_LightController);
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
	if (obj == nullptr) {
		std::cout << "Invalid object pointer!" << std::endl;
		return 1;
	}

	// Print debug information before removal
	std::cout << "Removing object: " << obj->m_DisplayName << std::endl;
	std::cout << "Vector size before removal: " << m_SceneObjects.size() << std::endl;

	auto removeIterator = std::find(m_SceneObjects.begin(), m_SceneObjects.end(), obj);

	if (removeIterator != m_SceneObjects.end()) {
		std::cout << "Object found in vector, deleting it." << std::endl;
		delete* removeIterator; // Delete the object
		m_SceneObjects.erase(removeIterator); // Erase the element from the vector

		// Verify removal
		std::cout << "Vector size after removal: " << m_SceneObjects.size() << std::endl;
		return 0;
	}

	std::cout << "Scene: " << obj->m_ObjectID << " is not part of the scene!" << std::endl;
	return 1;
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
	glEnable(GL_DEPTH_TEST);

	m_ShadowMap->shadowPass();

	// reset viewport after shadow pass
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/*
	{ // Uncomment this to render the debug quads for spotlights (cannot be rendered with the actual scene)
		//You may not be aligned with the debug quads.
		//Use regular controls to move
		m_ShadowMap->debugDepthShader.use();
		m_ShadowMap->debugDepthShader.setMat4("projection", proj);
		m_ShadowMap->debugDepthShader.setMat4("view", view);
		m_ShadowMap->debugShadowMap();
	}*/

	{ // Uncomment this to render the scene


		//Draw Point Light Lamp (light spheres)
		{
			pointLightShader->use();
			pointLightShader->setMat4("projection", proj);
			pointLightShader->setMat4("view", view);
			//Draw Lights (just the object, lighting is done in shader)
			m_LightController->drawLighting();
		}

		// Draw main scene and update physics
		{
			lightingShader->use();
			lightingShader->setMat4("projection", proj);
			lightingShader->setMat4("view", view);
			lightingShader->setVec3("viewPos", mainCamera->m_LookFrom);

			//Apply Textures to scene for shadow maps
			m_ShadowMap->updateShaderUniforms(*lightingShader);

			//Update physics
			m_PhysicsWorld->step(static_cast<float>(glfwGetTime()), deltaTime);

			//Draw Objects
			for (Object* element : m_SceneObjects){
				element->Draw(*lightingShader);
			}

			m_SkyBox->draw(proj);


		}
	}

	glDisable(GL_DEPTH_TEST);

	//std::cout << "Texture Units used this frame: " << TextureManager::getCurrentUnit() << "\n";
	//Reset current unit to zero for next render pass
	TextureManager::reset();
}