#include "Object.h"

Object::Object() : m_ebo(0), m_vao(0), m_vbo(0)
{
    //Set default Cube properties
    m_Ambient = glm::vec3(0.0f);
    m_Diffuse = glm::vec3(1.0f);
    m_Specular = glm::vec3(0.0f);
    m_Position = glm::vec3(0.0f);
    m_Rotation = glm::vec3(0.0f);
    m_Scale = glm::vec3(1.0f);
    m_Model = glm::mat4(1.0f);
    m_ObjectID = 0;

    m_Force = glm::vec3(0.0f);
    m_Velocity = glm::vec3(0.0f);
    m_Mass = glm::vec3(1.0f);

    m_HasTexture = false;
    m_DiffuseMap = nullptr;
    m_SpecularMap = nullptr;
    enablePhysics = false;
    isPhysicsObject = false;

    startFall = 0.0f;

    updateObject();
}

Object::~Object()
{
    std::cout << "Destructor called on " << m_ObjectID << std::endl;
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ebo);
    m_vao = 0;
    m_vbo = 0;
    m_ebo = 0;
    //Texture Properties
    delete m_DiffuseMap;
    delete m_SpecularMap;
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


glm::vec3 Object::getAmbient() const
{
    return m_Ambient;
}

glm::vec3 Object::getDiffuse() const
{
    return m_Diffuse;
}

glm::vec3 Object::getSpecular() const
{
    return m_Specular;
}


void Object::setPhysics()
{
    if (enablePhysics)
        startFall = glfwGetTime();
    setVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
}

void Object::setVelocity(glm::vec3 v)
{
    m_Velocity = v;
}

void Object::setPosition(glm::vec3 newPosition)
{
    m_Position = newPosition;
    updateObject();
}

//Takes a glm::vec3 of x, y, z rotations in degrees, the 
void Object::setRotation(glm::vec3 newRotation)
{
    m_Rotation = newRotation;
    //m_RotationAxis = axis;
    //m_RotationIndex = index;
    updateObject();
}

void Object::setScale(glm::vec3 newScale)
{
    m_Scale = newScale;
    updateObject();
}

void Object::translatePosition(glm::vec3 newPosition)
{
    m_Position += newPosition;
    updateObject();
}

void Object::updateObject()
{
    m_Model = glm::mat4(1.0f);
    // Apply translation transformation
    m_Model = glm::translate(m_Model, m_Position);
    // Apply scaling transformation
    m_Model = glm::scale(m_Model, m_Scale);


    // Apply Rotation
    float theta_x = glm::radians(m_Rotation[0]);
    float theta_y = glm::radians(m_Rotation[1]);
    float theta_z = glm::radians(m_Rotation[2]);

    //GLM Rotation
    //m_Model = glm::rotate(m_Model, theta_x, glm::vec3(1.0f, 0.0f, 0.0f));
    //m_Model = glm::rotate(m_Model, theta_y, glm::vec3(0.0f, 1.0f, 0.0f));
    //m_Model = glm::rotate(m_Model, theta_z, glm::vec3(0.0f, 0.0f, 1.0f));

    //Manual Rotation
    glm::mat4 x_rotation = glm::mat4(1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, glm::cos(theta_x), glm::sin(theta_x), 0.0f,
        0.0f, -glm::sin(theta_x), glm::cos(theta_x), 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);

    glm::mat4 y_rotation = glm::mat4(glm::cos(theta_y), 0.0f, -glm::sin(theta_y), 0.0f,
        0.0f,1.0f, 0.0f, 0.0f,
        glm::sin(theta_y), 0.0f, glm::cos(theta_y), 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);

    glm::mat4 z_rotation = glm::mat4(glm::cos(theta_z), glm::sin(theta_z), 0.0f, 0.0f,
        -glm::sin(theta_z), glm::cos(theta_z), 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);
    m_Model *= x_rotation * y_rotation * z_rotation;

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
