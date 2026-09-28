#include "Objects/Object.h"

#include "Lighting/Shader.h"

#include <iostream>


Object::Object() : m_ebo(0), m_vao(0), m_vbo(0)
{
    //Set default Cube properties
    m_Material.setAmbient(glm::vec3(0.0f));
    m_Material.setDiffuse(glm::vec3(1.0f));
    m_Material.setSpecular(glm::vec3(0.0f)); 
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
}

void Object::updateTexture(const MaterialPaths& paths)
{
    // Three Scenarios
    // 1. Updating a texture of an Object that already has textures
    // 2. Updating a texture of an Object with no initial texture
    // 3. Updating the texture of a model object

    m_Material.setTextures(paths); 
}

void Object::assignIdentity(EntityId id, std::string name)
{
    m_EntityInfo.setInfo(name, id);
}

EntityId Object::id() const
{
    return m_EntityInfo.id;
}

const std::string& Object::displayName() const
{
    return m_EntityInfo.displayName;
}

void Object::ApplyMaterialUniforms(Shader& shader) {}

void Object::DrawMesh() {}
