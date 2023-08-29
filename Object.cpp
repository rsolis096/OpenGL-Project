#include "Object.h"
#define PI 3.141592653589793238462643383279502884197


void Object::setAmbient(glm::vec3 newAmbient)
{
    m_Ambient = newAmbient;
}

void Object::setDiffuse(glm::vec3 newDiffuse)
{
    m_Diffuse = newDiffuse;
}

void Object::setSpecular(glm::vec3 newSpecular)
{
    m_Specular = newSpecular;
}

void Object::setPosition(glm::vec3 newPosition)
{
    m_Position = newPosition;
    model = glm::mat4(1.0f);
    model = glm::translate(model, newPosition);
}

void Object::translatePosition(glm::vec3 newPosition)
{
    m_Position += newPosition;
    model = glm::mat4(1.0f);
    model = glm::translate(model, m_Position);
}
