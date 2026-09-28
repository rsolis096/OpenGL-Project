#pragma once

#include "Objects/Object.h"

class Cube : public Object
{
public:

    //Generate an Cube with no mesh data, default color data, no texture data, default world attributes
    Cube();
    Cube(const char* texturePathDiffuse, const char* texturePathSpecular);

    //Methods
    void Draw(Shader& shader) override;
    void ShadowPassDraw(Shader& shader) override;
    void DrawGeometryPass(Shader& shader) override;
    ObjectType GetType() const override { return ObjectType::Cube; }

private:
    //Used for construction of primitives
    void buildCube();
    unsigned int m_IndexCount = 0;

protected:
    void ApplyMaterialUniforms(Shader& shader) override;
    void DrawMesh() override;

};
