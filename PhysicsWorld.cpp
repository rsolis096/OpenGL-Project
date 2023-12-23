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

void PhysicsWorld::step(float totalTime, float deltaTime)
{
	for (Object* obj : m_Objects)
	{
		if (obj->enablePhysics)
		{
			float airTime = totalTime - obj->startFall;
			obj->m_Force += (obj->m_Mass) * (m_Gravity); //Units in Newtons

			//Calculate the current velocity given the amount of time spent in air total (acceleration)
			obj->m_Velocity += (obj->m_Force / obj->m_Mass) * (airTime * 0.05f); //(Newton (kg * (m/s^2)) / kg) * frametime = velocity

			//Terminal Velocity in y direction
			if (obj->m_Velocity[1] > 54)
				obj->m_Velocity[1] = 54;

			//Calculate the change in position given the time between frames
			obj->m_Position += obj->m_Velocity * (deltaTime * 0.05f); // m/s * s = position
			obj->m_Force = glm::vec3(0.0f, 0.0f, 0.0f);

			obj->updateModel();
		}

	}
}
