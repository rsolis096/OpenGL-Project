#pragma once

#include "PhysicsWorld.h"
#include "Model.h"
#include "SkyBox.h"

#include "Camera.h"

#include "LightController.h"
#include "ShadowMap.h"

#include "TextureManager.h"
#include <typeinfo>
#include <string>

class Scene
{
public:

	//Total Objects in scene
	unsigned short m_SceneObjectCount;
	float fps;

	Camera* mainCamera;

	LightController* m_LightController;
	PhysicsWorld* m_PhysicsWorld;

	Shader* cubeMapShader; //skybox
	Shader* lightingShader;
	Shader* pointLightShader;

	ShadowMap* m_ShadowMap;
	SkyBox* m_SkyBox;

	std::vector<Object*> m_SceneObjects;

	std::vector<Model*> sceneModels;

	//Constructors
	Scene(Camera*);


	int addObject(Object* obj);
	int removeObject(Object* obj);
	void removeAllObjects();

	void addLightController(LightController* lc);
	void removeLightController();

	//void addShader(Shader&);

	void drawScene(float, glm::mat4&, glm::mat4&);

};

