#include "Object.h"

Object::Object() : m_ebo(0), m_vao(0), m_vbo(0)
{
    //Set default Cube properties
    m_Ambient = glm::vec3(0.2f);
    m_Diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
    m_Specular = glm::vec3(0.1f);
    m_Position = glm::vec3(0.0f, 0.0f, 0.0f);
    m_Scale = glm::vec3(1.0f, 1.0f, 1.0f);
    m_Model = glm::mat4(1.0f);
    m_Type = "Undefined Object";

    m_Force = glm::vec3(0.0f, 0.0f, 0.0f);
    m_Velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    m_Mass = glm::vec3(1, 1, 1);

    m_hasTexture = false;
    m_DiffuseMap = nullptr;
    m_SpecularMap = nullptr;

    updateModel();
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
    updateModel();
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

void Object::buildInterleavedVertices()
{
    std::vector<float>().swap(m_InterleavedVertices);

    std::size_t i, j;
    std::size_t count = m_Vertices.size();
    for (i = 0, j = 0; i < count; i += 3)
    {
        m_InterleavedVertices.push_back(m_Vertices[i]);
        m_InterleavedVertices.push_back(m_Vertices[i + 1]);
        m_InterleavedVertices.push_back(m_Vertices[i + 2]);

        m_InterleavedVertices.push_back(m_Normals[i]);
        m_InterleavedVertices.push_back(m_Normals[i + 1]);
        m_InterleavedVertices.push_back(m_Normals[i + 2]);

    }
}

void Object::buildInterleavedVerticesWithTexCoords()
{
    std::vector<float>().swap(m_InterleavedVertices);

    std::size_t i, j;
    std::size_t count = m_Vertices.size();
    for (i = 0, j = 0; i < count; i += 3, j += 2)
    {
        m_InterleavedVertices.push_back(m_Vertices[i]);
        m_InterleavedVertices.push_back(m_Vertices[i + 1]);
        m_InterleavedVertices.push_back(m_Vertices[i + 2]);

        m_InterleavedVertices.push_back(m_Normals[i]);
        m_InterleavedVertices.push_back(m_Normals[i + 1]);
        m_InterleavedVertices.push_back(m_Normals[i + 2]);

        m_InterleavedVertices.push_back(m_TexCoords[j]);
        m_InterleavedVertices.push_back(m_TexCoords[j + 1]);
    }
}
