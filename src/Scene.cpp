#include "Scene.h"

//Static members
unsigned int Scene::s_SCREEN_HEIGHT = 1080;
unsigned int Scene::s_SCREEN_WIDTH = 1920;
//Forward Declarations
class PhysicsWorld;

Scene::Scene(Camera* mC)
{
	m_SceneObjectCount = 0;
	m_fps = 0;
	m_mainCamera = mC;

	m_deferredLightingShader = new Shader("shaders/deferredLighting.vert", "shaders/deferredLighting.frag");

	m_cubeMapShader = new Shader("shaders/skyBoxShader.vert", "shaders/skyBoxShader.frag");
	m_gBufferShader = new Shader("shaders/gBuffer.vert", "shaders/gBuffer.frag");
	m_gBufferDebugShader = new Shader("shaders/gBufferDebug.vert", "shaders/gBufferDebug.frag");
	
	m_SSAOShader = new Shader("shaders/ssao.vert", "shaders/ssao.frag");
	m_SSAOBlurShader = new Shader("shaders/ssaoBlur.vert", "shaders/ssaoBlur.frag");
	
	InitializeDeferredRenderingShaders();

	//Enable this to view normals
	//lightingShader = new Shader("shaders/visualizeNormals.vert", "shaders/visualizeNormals.frag", "shaders/visualizeNormals.gs");

	//Enable this to render purely point light shadows (for implementing shadows to main shader)
	//lightingShader = new Shader("shaders/testShader.vs", "shaders/testShader.fs");

	m_pointLightShader = new Shader("shaders/pointLightShader.vert", "shaders/pointLightShader.frag");
	m_PhysicsWorld = new PhysicsWorld();

	m_LightController = new LightController(m_deferredLightingShader, m_pointLightShader, m_mainCamera, this);
	m_shadowMap = new ShadowMap(&m_sceneObjects, m_LightController);
	m_skyBox = new SkyBox(*m_cubeMapShader, m_mainCamera);

	m_gBuffer = new GBuffer(s_SCREEN_WIDTH, s_SCREEN_HEIGHT);
	m_SSAOBuffer = new SSAOBuffer(s_SCREEN_WIDTH, s_SCREEN_HEIGHT);
	m_SSAOBlurBuffer = new SSAOBlurBuffer(s_SCREEN_WIDTH, s_SCREEN_HEIGHT);

	glCheckError();
}

void Scene::InitializeDeferredRenderingShaders()
{
	{ // deferred lighting shader

		if (m_gBufferDebugShader != nullptr)
		{
			m_deferredLightingShader->use();
			m_deferredLightingShader->setInt("gPosition", 0);
			m_deferredLightingShader->setInt("gNormal", 1);
			m_deferredLightingShader->setInt("gAlbedoSpec", 2);

			m_deferredLightingShader->setInt("dirLight.shadowMap", 5);

			for (int i = 0; i < 8; i++)
			{
				m_deferredLightingShader->setInt("spotLights[" + std::to_string(i) + "].shadowMap", 6 + i);
				m_deferredLightingShader->setInt("pointLights[" + std::to_string(i) + "].shadowMap", 14 + i);
			}
		}
	}

	{ // SSAO Shader
		if (m_SSAOShader != nullptr)
		{
			m_SSAOShader->use();
			m_SSAOShader->setInt("gPosition", 0);
			m_SSAOShader->setInt("gNormal", 1);
			m_SSAOShader->setInt("texNoise", 2);
		}
	}

	{ // SSAO Shader
		if (m_SSAOBlurShader != nullptr)
		{
			m_SSAOBlurShader->use();
			m_SSAOBlurShader->setInt("ssaoInput", 0);
		}
	}
}

//Add an object to the scene (only objects part of a scene are rendered)
int Scene::addObject(Object* obj)
{
	
	if (obj == nullptr)
		return 1;
	
	std::vector<Object*>::iterator it = std::find(m_sceneObjects.begin(), m_sceneObjects.end(), obj);
	if (it == m_sceneObjects.end())
	{
		m_sceneObjects.push_back(obj);
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
	std::cout << "Vector size before removal: " << m_sceneObjects.size() << std::endl;

	auto removeIterator = std::find(m_sceneObjects.begin(), m_sceneObjects.end(), obj);

	if (removeIterator != m_sceneObjects.end()) {
		std::cout << "Object found in vector, deleting it." << std::endl;
		delete* removeIterator; // Delete the object
		m_sceneObjects.erase(removeIterator); // Erase the element from the vector

		// Verify removal
		std::cout << "Vector size after removal: " << m_sceneObjects.size() << std::endl;
		return 0;
	}

	std::cout << "Scene: " << obj->m_ObjectID << " is not part of the scene!" << std::endl;
	return 1;
}

//Remove all objects from the scene
void Scene::removeAllObjects()
{
	for (auto it = m_sceneObjects.begin(); it != m_sceneObjects.end(); ++it)
		delete* it;
	m_sceneObjects.clear();
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

	m_shadowMap->ShadowPass();

	// Update simulation once.
	m_PhysicsWorld->step(static_cast<float>(glfwGetTime()), deltaTime);

	glCheckError();

	// Geometry pass: render camera-visible surfaces into G-buffer.
	{
		m_gBuffer->Bind();

		glViewport(0, 0, s_SCREEN_WIDTH, s_SCREEN_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_gBufferShader->use();
		m_gBufferShader->setMat4("projection", proj);
		m_gBufferShader->setMat4("view", view);

		for (Object* element : m_sceneObjects)
		{
			element->DrawGeometryPass(*m_gBufferShader);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glCheckError();
	}

	{ // SSAO pass

		{ // generate SSAO texture
			
			m_SSAOBuffer->Bind();
			glViewport(0, 0, s_SCREEN_WIDTH, s_SCREEN_HEIGHT);
			glClear(GL_COLOR_BUFFER_BIT);

			m_SSAOShader->use();

			// Send kernel + rotation 
			for (unsigned int i = 0; i < 64; ++i)
			{
				m_SSAOShader->setVec3("samples[" + std::to_string(i) + "]", m_SSAOBlurBuffer->GetSSAOKernel()[i]);
			}
			m_SSAOShader->setMat4("projection", proj);
			m_SSAOShader->setMat4("view", view);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_gBuffer->GetPositionTexture());

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_gBuffer->GetNormalTexture());

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, m_SSAOBlurBuffer->GetNoiseTexture());

			RenderFullscreenQuad();
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

		}

		{ // SSAO blur

			m_SSAOBlurBuffer->Bind();

			glViewport(0, 0, s_SCREEN_WIDTH, s_SCREEN_HEIGHT);
			glClear(GL_COLOR_BUFFER_BIT);

			m_SSAOBlurShader->use();

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_SSAOBuffer->GetColorBuffer());

			RenderFullscreenQuad();
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
	}



	// lighting pass
	{
		glViewport(0, 0, s_SCREEN_WIDTH, s_SCREEN_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDisable(GL_DEPTH_TEST);
		m_deferredLightingShader->use();
		//m_deferredLightingShader->setVec3("viewPos", m_mainCamera->m_LookFrom);

		m_gBuffer->BindTextures(*m_deferredLightingShader);
		m_SSAOBlurBuffer->BindTextures(*m_deferredLightingShader);
		m_shadowMap->UpdateShaderUniforms(*m_deferredLightingShader);

		//m_gBufferDebugShader->use();
		//m_gBuffer->BindTextures(*m_gBufferDebugShader);
		//m_gBuffer->BindGenericTexture(m_SSAOBuffer->GetColorBuffer(), "gOther", *m_gBufferDebugShader);

		//m_gBufferDebugShader->setInt("debugMode", 0); // albedo first
		//m_gBufferDebugShader->setInt("debugMode", 1); // normals
		//m_gBufferDebugShader->setInt("debugMode", 2); // world position
		//m_gBufferDebugShader->setInt("debugMode", 3); // grayscale specular
		//m_gBufferDebugShader->setInt("debugMode", 4); // distance-from-origin gradient
		//m_gBufferDebugShader->setInt("debugMode", 5); 

		//m_gBufferDebugShader->setInt("debugMode", 2); // world position
		//m_gBufferDebugShader->setFloat("positionDebugScale", 20.0f);

		RenderFullscreenQuad();

		glEnable(GL_DEPTH_TEST);
		glCheckError();
	}

	// Optional: draw light source spheres after lighting pass (overlayed but still helpful)
	{
		m_pointLightShader->use();
		m_pointLightShader->setMat4("projection", proj);
		m_pointLightShader->setMat4("view", view);
		m_LightController->drawLighting();
	}

	{// Skybox

		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_gBuffer->GetFBO());
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		glBlitFramebuffer(
			0, 0, s_SCREEN_WIDTH, s_SCREEN_HEIGHT,
			0, 0, s_SCREEN_WIDTH, s_SCREEN_HEIGHT,
			GL_DEPTH_BUFFER_BIT,
			GL_NEAREST
		);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		m_skyBox->draw(proj);

	}

	glDisable(GL_DEPTH_TEST);
}

void Scene::RenderFullscreenQuad()
{
	if (m_quadVAO == 0)
	{
		float quadVertices[] =
		{
			// positions        // texCoords
			-1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f,  1.0f, 0.0f
		};

		glGenVertexArrays(1, &m_quadVAO);
		glGenBuffers(1, &m_quadVBO);

		glBindVertexArray(m_quadVAO);

		glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);


		glCheckError();

		// position
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT,GL_FALSE, 5 * sizeof(float), (void*)0);


		glCheckError();

		// texCoords
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glBindVertexArray(0);

		glCheckError();

	}

	glBindVertexArray(m_quadVAO);

	glCheckError();

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glCheckError();

	glBindVertexArray(0);

	glCheckError();

}