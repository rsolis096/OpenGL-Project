#pragma once

#include "Objects/Mesh.h"
#include "Objects/MeshBuilder.h"
#include "Objects/Object.h"

class Primitive : public Object
{
public:
    Primitive(PrimitiveKind kind);
    Primitive(PrimitiveKind kind, const char* texturePathDiffuse, const char* texturePathSpecular);

    void Draw(Shader& shader) override;
    void ShadowPassDraw(Shader& shader) override;
    void DrawGeometryPass(Shader& shader) override;
    ObjectType GetType() const override;

    PrimitiveKind kind() const noexcept;

private:
    PrimitiveKind m_Kind;
    Mesh m_Mesh;

    void ApplyMaterialUniforms(Shader& shader) override;
    void draw(Shader& shader, bool applyMaterial);
};
