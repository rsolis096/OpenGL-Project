#pragma once

#include <string>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

using EntityId = std::uint64_t;
using PlayerId = std::uint32_t;

struct EntityInfo {
    EntityId id = 0;
    std::string displayName;

    void setInfo(std::string name, std::uint64_t id)
    {
        this->id = id;
        displayName = name;
    }
};

enum class Authority {
    Local,
    Host,
    Remote
};

enum class ObjectType
{
    Cube,
    Model,
    Sphere,
    Plane,
    Object,
};

struct NetworkIdentity {
    PlayerId owner = 0;
    Authority authority = Authority::Local;
    bool replicateTransform = true;
};

struct Transform {

    glm::vec3 position{ 0.0f };
    glm::vec3 rotation{ 0.0f };
    glm::vec3 scale{ 1.0f };

    glm::mat4 matrix() const
    {
        glm::mat4 model = glm::mat4(1.0f);

        // Apply translation transformation
        model = glm::translate(model, position);

        // Apply scaling transformation
        model = glm::scale(model, scale);

        // Apply Rotation
        float theta_x = glm::radians(rotation[0]);
        float theta_y = glm::radians(rotation[1]);
        float theta_z = glm::radians(rotation[2]);

        //GLM Rotation
        model = glm::rotate(model, theta_x, glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, theta_y, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, theta_z, glm::vec3(0.0f, 0.0f, 1.0f));

        return model;
    }
};

struct Material {
    glm::vec3 ambient{ 0.0f };
    glm::vec3 diffuse{ 1.0f };
    glm::vec3 specular{ 0.0f };

    std::unique_ptr<Texture> diffuseMap;
    std::unique_ptr<Texture> specularMap;

    bool hasTextures() const;
    void setTextures(const MaterialPaths& paths);
};

struct MaterialPaths {
    std::string diffuse;
    std::string specular;
};