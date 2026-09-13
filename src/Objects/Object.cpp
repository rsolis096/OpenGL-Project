#include "Objects/Object.h"

#include "Lighting/Shader.h"

#include <GLFW/glfw3.h>

#include <iostream>

unsigned int Object::objectCount = 0;

Object::Object() : m_ebo(0), m_vao(0), m_vbo(0)
{
    //Set default Cube properties
    m_Material.setAmbient(glm::vec3(0.0f));
    m_Material.setDiffuse(glm::vec3(1.0f));
    m_Material.setSpecular(glm::vec3(0.0f));

    m_Force = glm::vec3(0.0f);
    m_Velocity = glm::vec3(0.0f);
    m_Mass = glm::vec3(1.0f);

    enablePhysics = false;
    isPhysicsObject = false;

    startFall = 0.0f;
}

Object::~Object()
{
    std::cout << "Destructor called on " << m_EntityInfo.id << std::endl;
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ebo);
    m_vao = 0;
    m_vbo = 0;
    m_ebo = 0;

    // Clear vectors
    m_Vertices.clear();
    m_Vertices.shrink_to_fit();
    m_TexCoords.clear();
    m_TexCoords.shrink_to_fit();
    m_Normals.clear();
    m_Normals.shrink_to_fit();
    m_Indices.clear();
    m_Indices.shrink_to_fit();
    m_InterleavedVertices.clear();
    m_InterleavedVertices.shrink_to_fit();
}



void Object::updateTexture(std::vector<std::string> texturePaths)
{
    // Three Scenarios
    // 1. Updating a texture of an Object that already has textures
    // 2. Updating a texture of an Object with no initial texture
    // 3. Updating the texture of a model object

    MaterialPaths paths;
    paths.diffuse = texturePaths[0];
    paths.specular = texturePaths[1];
    m_Material.setTextures(paths);
    
}

void Object::setPhysics()
{
    if (enablePhysics)
    {
        startFall = glfwGetTime();
    }

    setVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
}

void Object::setVelocity(glm::vec3 v)
{
    m_Velocity = v;
}

//Combines vertices and normals
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

void Object::ApplyMaterialUniforms(Shader& shader) {}

void Object::DrawMesh() {}
