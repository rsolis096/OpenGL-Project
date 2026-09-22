#include "Lighting/Light.h"

#include <algorithm>
#include <utility>

Light::Light(
    Shader* lightingShader,
    std::string displayName,
    const glm::vec3& ambient,
    const glm::vec3& diffuse,
    const glm::vec3& specular,
    float intensity)
    : m_LightingShader(lightingShader),
      m_DisplayName(std::move(displayName)),
      m_Ambient(ambient),
      m_Diffuse(diffuse),
      m_Specular(specular),
      m_Intensity(std::max(intensity, 0.0f))
{
}

const std::string& Light::getDisplayName() const
{
    return m_DisplayName;
}

glm::vec3 Light::getAmbient() const
{
    return m_Ambient;
}

glm::vec3 Light::getDiffuse() const
{
    return m_Diffuse;
}

glm::vec3 Light::getSpecular() const
{
    return m_Specular;
}

float Light::getIntensity() const
{
    return m_Intensity;
}

void Light::setAmbient(const glm::vec3& ambient)
{
    m_Ambient = ambient;
    updateCommonShaderUniforms();
}

void Light::setDiffuse(const glm::vec3& diffuse)
{
    m_Diffuse = diffuse;
    updateCommonShaderUniforms();
}

void Light::setSpecular(const glm::vec3& specular)
{
    m_Specular = specular;
    updateCommonShaderUniforms();
}

void Light::setIntensity(float intensity)
{
    m_Intensity = std::max(intensity, 0.0f);
    updateCommonShaderUniforms();
}
