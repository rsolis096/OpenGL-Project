#pragma once

#include "Objects/Object.h"

class Sphere : public Object
{
public:

    Sphere();
    Sphere(const char* texturePathDiffuse, const char* texturePathSpecular);

    //Methods
    void Draw(Shader& shader) override;
    void ShadowPassDraw(Shader& shader) override;    
    void DrawGeometryPass(Shader& shader) override;
    ObjectType GetType() const override { return ObjectType::Sphere; }

private:
    //Used for construction of primitives
    void buildSphere();

protected:
    void ApplyMaterialUniforms(Shader& shader) override;
    void DrawMesh() override;

};
