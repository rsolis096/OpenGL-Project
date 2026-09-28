#include "Objects/Primitive.h"

#include "Lighting/Shader.h"
#include "Objects/Texture.h"

Primitive::Primitive(PrimitiveKind kind) :
    Object(),
    m_Kind(kind),
    m_Mesh(MeshBuilder::build(kind))
{
}

Primitive::Primitive(
    PrimitiveKind kind,
    const char* texturePathDiffuse,
    const char* texturePathSpecular) :
    Primitive(kind)
{
    m_Material.setTextures({ texturePathDiffuse, texturePathSpecular });
}

void Primitive::Draw(Shader& shader)
{
    draw(shader, true);
}

void Primitive::ShadowPassDraw(Shader& shader)
{
    draw(shader, false);
}

void Primitive::DrawGeometryPass(Shader& shader)
{
    draw(shader, true);
}

ObjectType Primitive::GetType() const
{
    switch (m_Kind)
    {
    case PrimitiveKind::Cube:
        return ObjectType::Cube;
    case PrimitiveKind::Sphere:
        return ObjectType::Sphere;
    case PrimitiveKind::Plane:
        return ObjectType::Plane;
    case PrimitiveKind::Cylinder:
        return ObjectType::Cylinder;
    case PrimitiveKind::Cone:
        return ObjectType::Cone;
    case PrimitiveKind::Torus:
        return ObjectType::Torus;
    default:
        return ObjectType::Object;
    }
}

PrimitiveKind Primitive::kind() const noexcept
{
    return m_Kind;
}

void Primitive::ApplyMaterialUniforms(Shader& shader)
{
    shader.setMaterial(m_Material);

    if (!m_Material.hasTextures())
        return;

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_Material.m_DiffuseMap->ID);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_Material.m_SpecularMap->ID);
}

void Primitive::draw(Shader& shader, bool applyMaterial)
{
    shader.use();
    shader.setMat4("model", m_Transform.matrix());

    if (applyMaterial)
        ApplyMaterialUniforms(shader);

    m_Mesh.DrawMesh();
}
