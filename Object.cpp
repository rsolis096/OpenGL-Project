#include "Object.h"
#define PI 3.141592653589793238462643383279502884197

Object::Object() : m_ebo(0), m_vao(0), m_vbo(0)
{
    //Set default Cube properties
    m_Ambient = glm::vec3(0.2f);
    m_Diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
    m_Specular = glm::vec3(0.1f);
    m_Position = glm::vec3(1.0f, 1.0f, 1.0f);
    m_Scale = glm::vec3(1.0f, 1.0f, 1.0f);
    m_Model = glm::mat4(1.0f);
    m_Type = "Undefined Object";

    m_hasTexture = false;
    m_DiffuseMap = nullptr;
    m_SpecularMap = nullptr;
}


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
    //m_Model = glm::mat4(1.0f);
    m_Model = glm::translate(m_Model, newPosition);
}

void Object::setScale(glm::vec3 newScale)
{
    m_Scale = newScale;
    updateModel();
}

void Object::translatePosition(glm::vec3 newPosition)
{
    m_Position += newPosition;
    updateModel();
}

void Object::updateModel()
{
    m_Model = glm::mat4(1.0f);

    // Apply translation transformation
    m_Model = glm::translate(m_Model, m_Position);
    // Apply scaling transformation
    m_Model = glm::scale(m_Model, m_Scale);


}