#include "Objects/Transform.h"

void Transform::setPosition(glm::vec3 newPosition)
{
    m_Position = newPosition;
    m_Revision++;
}

void Transform::setRotation(glm::vec3 newRotation)
{
     m_Rotation = newRotation;
     m_Revision++;
}

void Transform::setScale(glm::vec3 newScale)
{
    m_Scale = newScale;
    m_Revision++;
}

void Transform::translatePosition(glm::vec3 newPosition)
{
    m_Position += newPosition;
    m_Revision++;
}

std::uint64_t Transform::revision() const
{
    return m_Revision;
}

glm::mat4 Transform::matrix() const
{
    glm::mat4 model = glm::mat4(1.0f);

    model = glm::translate(model, m_Position);

    float theta_x = glm::radians(m_Rotation[0]);
    float theta_y = glm::radians(m_Rotation[1]);
    float theta_z = glm::radians(m_Rotation[2]);
    model = glm::rotate(model, theta_x, { 1.0f, 0.0f, 0.0f });
    model = glm::rotate(model, theta_y, { 0.0f, 1.0f, 0.0f });
    model = glm::rotate(model, theta_z, { 0.0f, 0.0f, 1.0f });

    model = glm::scale(model, m_Scale);

    return model;
}

const glm::vec3& Transform::position() const
{
    return m_Position;
}

const glm::vec3& Transform::rotation() const
{
    return m_Rotation;
}

const glm::vec3& Transform::scale() const
{
    return m_Scale;
}
