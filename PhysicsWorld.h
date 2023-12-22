#pragma once

#include "Object.h"


class PhysicsWorld
{
private:
	std::vector<Object*> m_Objects;
	glm::vec3 m_Gravity = glm::vec3(0.0f, -9.81f, 0.0f);

public:
	PhysicsWorld();
	void addObject(Object* obj);
	void removeObject(Object* obj);
	void step(float dt);
};