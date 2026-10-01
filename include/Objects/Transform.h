#pragma once

#include <cstdint>

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Transform {

private:

    glm::vec3 m_Position{ 0.0f };
    glm::vec3 m_Rotation{ 0.0f };
    glm::vec3 m_Scale{ 1.0f };

    std::uint64_t m_Revision = 0;

public:

    glm::mat4 matrix() const;

    const glm::vec3& position() const;
    const glm::vec3& rotation() const;
    const glm::vec3& scale() const;

    void setPosition(glm::vec3 newPosition);
    void setRotation(glm::vec3 newRotation);
    void setScale(glm::vec3 newScale);
    void translatePosition(glm::vec3 newPosition);

    // Revision is updated when a setter makes a change
    std::uint64_t revision() const;
};
