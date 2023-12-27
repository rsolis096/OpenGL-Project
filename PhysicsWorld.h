#pragma once

#include "Object.h"

class PhysicsWorld
{
private:
	glm::vec3 m_Gravity = glm::vec3(0.0f, -9.810f, 0.0f);
	std::vector<Object*> m_PhysicsObjects;

public:	
	PhysicsWorld();
	int addObject(Object* obj);
	int removeObject(Object* obj);
	void removeAllObjects();
	void step(float totalTime, float deltaTime);
};