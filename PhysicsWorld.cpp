#include "PhysicsWorld.h"

PhysicsWorld::PhysicsWorld()
{

}

void PhysicsWorld::addObject(Object* obj)
{
	m_Objects.push_back(obj);
}

void PhysicsWorld::removeObject(Object* obj)
{
	std::vector<Object*>::iterator it = m_Objects.begin();
	while (it != m_Objects.end())
	{
		if (*it == obj)
			m_Objects.erase(it);
		else
			it++;
	}
}

void PhysicsWorld::step(float deltaTime)
{;
	for (Object* obj : m_Objects)
	{
		obj->m_Force += (obj->m_Mass) * (m_Gravity *0.001f); //Units in Newtons
		obj->m_Velocity += (obj->m_Force / obj->m_Mass) * deltaTime; //(Newton / Kg) * frametime = velocity
		obj->m_Position += obj->m_Velocity * deltaTime; // m/s * s = position
		obj->m_Force = glm::vec3(0.0f, 0.0f, 0.0f);

		obj->updateModel();
	}
}
