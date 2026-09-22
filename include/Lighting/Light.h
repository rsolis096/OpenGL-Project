#pragma once

#include <glm/vec3.hpp>

#include <string>

class Shader;

class Light
{
public:
    virtual ~Light() = default;

    const std::string& getDisplayName() const;

    glm::vec3 getAmbient() const;
    glm::vec3 getDiffuse() const;
    glm::vec3 getSpecular() const;
    float getIntensity() const;

    void setAmbient(const glm::vec3& ambient);
    void setDiffuse(const glm::vec3& diffuse);
    void setSpecular(const glm::vec3& specular);
    void setIntensity(float intensity);

protected:
    Light(
        Shader* lightingShader,
        std::string displayName,
        const glm::vec3& ambient,
        const glm::vec3& diffuse,
        const glm::vec3& specular,
        float intensity = 1.0f);

    // Uploads the shared light state using the derived light's uniform layout
    virtual void updateCommonShaderUniforms() = 0;

    Shader* m_LightingShader;

private:
    std::string m_DisplayName;
    glm::vec3 m_Ambient;
    glm::vec3 m_Diffuse;
    glm::vec3 m_Specular;
    float m_Intensity;
};
