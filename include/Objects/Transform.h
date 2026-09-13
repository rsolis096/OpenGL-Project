#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Transform {

public:
    glm::vec3 m_Position{ 0.0f };
    glm::vec3 m_Rotation{ 0.0f };
    glm::vec3 m_Scale{ 1.0f };

    glm::mat4 matrix() const;
    void setPosition(glm::vec3 newPosition);
    void setRotation(glm::vec3 newRotation);
    void setScale(glm::vec3 newScale);
    void translatePosition(glm::vec3 newPosition);
};