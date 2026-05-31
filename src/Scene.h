#pragma once

#include "PhysicsWorld.h"
#include "Model.h"
#include "SkyBox.h"
#include "Camera.h"
#include "LightController.h"
#include "ShadowMap.h"

#include <typeinfo>
#include <string>

class GBuffer;


class Scene
{
public:
	static unsigned int SCREEN_WIDTH;
	static unsigned int SCREEN_HEIGHT;
	//Total Objects in scene
	unsigned short m_SceneObjectCount;
	float fps;

	Camera* mainCamera;

	GBuffer* m_GBuffer;

	LightController* m_LightController;
	PhysicsWorld* m_PhysicsWorld;

	Shader* cubeMapShader; //skybox
	Shader* lightingShader;
	Shader* pointLightShader;
	Shader* gBufferShader;
	Shader* gBufferDebugShader;

	ShadowMap* m_ShadowMap;
	SkyBox* m_SkyBox;

	std::vector<Object*> m_SceneObjects;

	std::vector<Model*> sceneModels;

	//Constructors
	Scene(Camera*);

	void InitLightingShaderSamplers(Shader& shader);


	int addObject(Object* obj);
	int removeObject(Object* obj);
	void removeAllObjects();

	void addLightController(LightController* lc);
	void removeLightController();

	//void addShader(Shader&);

	void drawScene(float, glm::mat4&, glm::mat4&);

private:
	unsigned int quadVAO = 0;
	unsigned int quadVBO = 0;

	void RenderFullscreenQuad();
};

