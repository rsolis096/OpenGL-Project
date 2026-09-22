#pragma once

#include <glm/glm.hpp>

#include <vector>

class Camera;
class GBuffer;
class LightController;
class Model;
class Object;
class Shader;
class ShadowMap;
class SkyBox;
class SSAOBuffer;
class SSAOBlurBuffer;

class Scene
{
public:
	static unsigned int s_SCREEN_WIDTH;
	static unsigned int s_SCREEN_HEIGHT;

	unsigned short m_SceneObjectCount; 	//Total Objects in scene
	float m_fps;

	Camera* m_mainCamera;

	LightController* m_LightController;
	// Shaders
	Shader* m_cubeMapShader; //skybox
	Shader* m_pointLightShader;

	Shader* m_deferredLightingShader;
	Shader* m_gBufferShader;
	Shader* m_gBufferDebugShader;

	Shader* m_SSAOShader;
	Shader* m_SSAOBlurShader;

	SkyBox* m_skyBox;
	std::vector<Object*> m_sceneObjects;
	std::vector<Model*> m_sceneModels;


	// FBOs
	GBuffer* m_gBuffer;
	SSAOBuffer* m_SSAOBuffer;
	SSAOBlurBuffer* m_SSAOBlurBuffer;
	ShadowMap* m_shadowMap;


	//Constructors
	Scene(Camera*);

	void InitializeDeferredRenderingShaders();


	int addObject(Object* obj);
	int removeObject(Object* obj);
	void removeAllObjects();

	void addLightController(LightController* lc);
	void removeLightController();

	//void addShader(Shader&);

	void drawScene(glm::mat4&, glm::mat4&);

private:
	unsigned int m_quadVAO = 0;
	unsigned int m_quadVBO = 0;

	void RenderFullscreenQuad();
};

