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
	gBufferShader = new Shader("shaders/gBuffer.vert", "shaders/gBuffer.frag");
	gBufferDebugShader = new Shader("shaders/gBufferDebug.vert", "shaders/gBufferDebug.frag");

	//Enable this to use the main shader
	lightingShader = new Shader("shaders/lightingShader.vert", "shaders/lightingShader.frag");
	InitLightingShaderSamplers(*lightingShader);

	//Enable this to view normals
	//lightingShader = new Shader("shaders/visualizeNormals.vert", "shaders/visualizeNormals.frag", "shaders/visualizeNormals.gs");

	//Enable this to render purely point light shadows (for implementing shadows to main shader)
	//lightingShader = new Shader("shaders/testShader.vs", "shaders/testShader.fs");

	pointLightShader = new Shader("shaders/pointLightShader.vert", "shaders/pointLightShader.frag");
	m_PhysicsWorld = new PhysicsWorld();

	m_LightController = new LightController(lightingShader, pointLightShader, mainCamera, this);
	m_ShadowMap = new ShadowMap(&m_SceneObjects, m_LightController);
	m_SkyBox = new SkyBox(*cubeMapShader, mainCamera);

	m_GBuffer = new GBuffer(SCREEN_WIDTH, SCREEN_HEIGHT);

	glCheckError();
}

void Scene::InitLightingShaderSamplers(Shader& shader)
{
	shader.use();

	shader.setInt("material.ambient", 0);
	shader.setInt("material.diffuse", 1);
	shader.setInt("material.specular", 2);

	shader.setInt("dirLight.shadowMap", 4);

	for (int i = 0; i < 8; i++)
	{
		shader.setInt("spotLights[" + std::to_string(i) + "].shadowMap", 5 + i);
		shader.setInt("pointLights[" + std::to_string(i) + "].shadowMap", 13 + i);
	}
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

	// Existing shadow maps stay.
	m_ShadowMap->shadowPass();

	// Update simulation once.
	m_PhysicsWorld->step(static_cast<float>(glfwGetTime()), deltaTime);

	// Geometry pass: render camera-visible surfaces into G-buffer.
	{
		m_GBuffer->BindForWriting();

		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		gBufferShader->use();
		gBufferShader->setMat4("projection", proj);
		gBufferShader->setMat4("view", view);

		for (Object* element : m_SceneObjects)
		{
			element->DrawGeometryPass(*gBufferShader);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	// Temporary debug pass: show G-buffer texture on screen.
	{
		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDisable(GL_DEPTH_TEST);

		gBufferDebugShader->use();
		m_GBuffer->BindForReading(*gBufferDebugShader);

		gBufferDebugShader->setInt("debugMode", 0); // albedo first
		//gBufferDebugShader->setInt("debugMode", 1); // normals
		//gBufferDebugShader->setInt("debugMode", 2); // world position
		//gBufferDebugShader->setInt("debugMode", 3); // grayscale specular
		//gBufferDebugShader->setInt("debugMode", 4); // distance-from-origin gradient

		//gBufferDebugShader->setInt("debugMode", 2); // world position
		gBufferDebugShader->setFloat("positionDebugScale", 20.0f);

		RenderFullscreenQuad();

		glEnable(GL_DEPTH_TEST);
	}

	// Optional: draw light source spheres after debug pass.
	{
		//pointLightShader->use();
		//pointLightShader->setMat4("projection", proj);
		//pointLightShader->setMat4("view", view);
		//m_LightController->drawLighting();
	}

	glDisable(GL_DEPTH_TEST);
}

void Scene::RenderFullscreenQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] =
		{
			// positions        // texCoords
			-1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f,  1.0f, 0.0f
		};

		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);

		glBindVertexArray(quadVAO);

		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

		// position
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT,GL_FALSE, 5 * sizeof(float), (void*)0);

		// texCoords
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glBindVertexArray(0);
	}

	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}