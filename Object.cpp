#include "Object.h"
#define PI 3.141592653589793238462643383279502884197

Object::Object() : diffuseMap(nullptr), specularMap(nullptr), basicTexture(nullptr), m_hasTexture(false)
{
    //Set default object properties
    m_Ambient = glm::vec3(0.0);
    m_Diffuse = glm::vec3(0.0f, 0.0f, 0.0f);
    m_Specular = glm::vec3(0.0f);
    m_Position = glm::vec3(0.0f, 0.0f, 0.0f);
    model = glm::mat4(1.0f);
    //Object Render Attributes
    m_vao = 0;
    m_vbo = 0;
    m_ebo = 0;
    m_useEBO = false;
    m_type = "null";
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


void Object::setAmbient(glm::vec3 newAmbient)
{
    //m_Shader.use();
    m_Ambient = newAmbient;
    //m_Shader.setVec3("object.ambient", m_Ambient);
}

void Object::setDiffuse(glm::vec3 newDiffuse)
{
    //m_Shader.use();
    m_Diffuse = newDiffuse;
    //m_Shader.setVec3("object.diffuse", m_Diffuse);
}

void Object::setSpecular(glm::vec3 newSpecular)
{
    //m_Shader.use();
    m_Specular = newSpecular;
    //m_Shader.setVec3("object.specular", m_Specular);
}

void Object::Draw(Shader& shader)
{

}

void Object::setPosition(glm::vec3 newPosition)
{
    m_Position = newPosition;
    model = glm::mat4(1.0f);
    model = glm::translate(model, newPosition);
    //m_Shader.use();
    //m_Shader.setMat4("model", model);
}

void Object::translatePosition(glm::vec3 newPosition)
{
    m_Position += newPosition;
    model = glm::mat4(1.0f);
    model = glm::translate(model, m_Position);
}
