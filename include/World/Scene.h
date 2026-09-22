#pragma once

#include <glm/glm.hpp>

#include <algorithm>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

#include <Objects/ObjectTypes.h>

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

	// FBOs
	GBuffer* m_gBuffer;
	SSAOBuffer* m_SSAOBuffer;
	SSAOBlurBuffer* m_SSAOBlurBuffer;
	ShadowMap* m_shadowMap;

	//Constructors
	Scene(Camera*);

	void InitializeDeferredRenderingShaders();

	void removeAllObjects();

	void addLightController(LightController* lc);
	void removeLightController();

	void drawScene(glm::mat4&, glm::mat4&);

	template<typename T, typename... Args>
	T& createEntity(Args&&... args);

	bool destroyEntity(EntityId id);

	Object* findEntity(EntityId id);
	const Object* findEntity(EntityId id) const;

	std::vector<std::unique_ptr<Object>> m_Entities;

private:
	unsigned int m_quadVAO = 0;
	unsigned int m_quadVBO = 0;

	void RenderFullscreenQuad();

	EntityId m_NextEntityId = 1;
	std::unordered_map<EntityId, Object*> m_EntityLookup;
	std::string makeDefaultName(const Object& entity, EntityId id) const;
};

template<typename T, typename... Args>
T& Scene::createEntity(Args&&... args)
{
	auto entity = std::make_unique<T>(std::forward<Args>(args)...);

	const EntityId id = m_NextEntityId++;
	entity->assignIdentity(id, makeDefaultName(*entity, id));

	T& result = *entity;
	m_EntityLookup.emplace(id, entity.get());
	m_Entities.push_back(std::move(entity));

	return result;
}

