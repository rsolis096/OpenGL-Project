#include <memory>
#include <string>

#include <glm/vec3.hpp>

#include "Texture.h"

struct MaterialPaths {
    std::string diffuse;
    std::string specular;
};

class Material {

private:
    glm::vec3 m_Ambient{ 0.0f };
    glm::vec3 m_Diffuse{ 1.0f };
    glm::vec3 m_Specular{ 0.0f };

public:

    std::unique_ptr<Texture> m_DiffuseMap;
    std::unique_ptr<Texture> m_SpecularMap;

    bool hasTextures() const;
    bool setTextures(const MaterialPaths& paths);

    void setAmbient(glm::vec3 newAmbient);
    void setDiffuse(glm::vec3 newDiffuse);
    void setSpecular(glm::vec3 newSpecular);

    glm::vec3 getAmbient() const;
    glm::vec3 getDiffuse() const;
    glm::vec3 getSpecular() const;
};
