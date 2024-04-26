#include "PhysicsWorld.h"

PhysicsWorld::PhysicsWorld()
{

}

int PhysicsWorld::addObject(Object* obj)
{
	std::vector<Object*>::iterator it = std::find(m_PhysicsObjects.begin(), m_PhysicsObjects.end(), obj);
	if (obj == nullptr)
		return 1;
	if (it == m_PhysicsObjects.end())
	{
		m_PhysicsObjects.push_back(obj);
		obj->isPhysicsObject = true;
		std::cout << "Added " << obj->m_ObjectID << " to physics world." << std::endl;
		return 0;
	}
	std::cout << "Object already in physics world!" << std::endl;
	return 1;
}

int PhysicsWorld::removeObject(Object* obj)
{
	//Remove all instances of obj
	std::vector<Object*>::iterator removeIterator = std::remove(m_PhysicsObjects.begin(), m_PhysicsObjects.end(), obj);
	if (removeIterator == m_PhysicsObjects.end())
	{
		std::cout << "Physics World: " << obj->m_ObjectID << " is not part of the Physics world!" << std::endl;
		return 1;
	}
	else
	{
		m_PhysicsObjects.erase(removeIterator, m_PhysicsObjects.end());
		obj->isPhysicsObject = false;
		std::cout << "Physics World: Removed " << obj->m_ObjectID << std::endl;
		return 0;
	}
}

//Remove all objects from the scene
void PhysicsWorld::removeAllObjects()
{
	m_PhysicsObjects.clear();
}

void PhysicsWorld::step(float totalTime, float deltaTime)
{
	for (Object* obj : m_PhysicsObjects)
	{
		if (obj->enablePhysics)
		{
			float airTime = totalTime - obj->startFall;
			obj->m_Force += (obj->m_Mass) * (m_Gravity); //Units in Newtons

			//Calculate the current velocity given the amount of time spent in air total (acceleration)
			obj->m_Velocity += (obj->m_Force / obj->m_Mass) * (airTime * 0.1f); //(Newton (kg * (m/s^2)) / kg) * frametime = velocity

			//Terminal Velocity in y direction
			if (obj->m_Velocity[1] > 54)
				obj->m_Velocity[1] = 54;

			//Calculate the change in position given the time between frames
			obj->m_Position += obj->m_Velocity * (deltaTime * 0.05f); // m/s * s = position
			obj->m_Force = glm::vec3(0.0f, 0.0f, 0.0f);

			obj->updateObject();
		}
	}
}

