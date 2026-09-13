#include "Objects/Material.h"

void Material::setAmbient(glm::vec3 newAmbient)
{
    m_Ambient = newAmbient;
}

void Material::setDiffuse(glm::vec3 newDiffuse)
{
    m_Diffuse = newDiffuse;
}

void Material::setSpecular(glm::vec3 newSpecular)
{
    m_Specular = newSpecular;
}

glm::vec3 Material::getAmbient() const
{
    return m_Ambient;
}

glm::vec3 Material::getDiffuse() const
{
    return m_Diffuse;
}

glm::vec3 Material::getSpecular() const
{
    return m_Specular;
}

bool Material::setTextures(const MaterialPaths& paths)
{
    if (hasTextures())
    {
        //Success flags
        int dSuccess = 1;
        int sSuccess = 1;

        if (paths.diffuse.size() > 0 && paths.specular.size() > 0)
        {
            dSuccess = m_DiffuseMap->updateTexture(paths.diffuse.c_str(), false);
            sSuccess = m_SpecularMap->updateTexture(paths.specular.c_str(), false);
        }

        if (dSuccess != 1 || sSuccess != 1)
        {
            //Texture failed to apply
            //As of right now, all textures are deleted
            m_DiffuseMap.reset();
            m_SpecularMap.reset();

            return false;
        }

        return true;
    }

    m_DiffuseMap = std::make_unique<Texture>(paths.diffuse.c_str(), false, "texture_diffuse");
    m_SpecularMap = std::make_unique<Texture>(paths.specular.c_str(), false, "texture_specular");

    if (m_DiffuseMap->ID == GL_INVALID_VALUE || m_SpecularMap->ID == GL_INVALID_VALUE)
    {
        //Texture failed to apply
        m_DiffuseMap.reset();
        m_SpecularMap.reset();

        return false;
    }

    return true;
}

bool Material::hasTextures() const
{
    return m_DiffuseMap != nullptr && m_SpecularMap != nullptr;
}