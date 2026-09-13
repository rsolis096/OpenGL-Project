#pragma once

#include "Objects/Object.h"

class Plane : public Object
{
public:

    //Generate an Plane with no mesh data, default color data, no texture data, default world attributes
    Plane();
	Plane(const char* texturePathDiffuse, const char* texturePathSpecular);

    void Draw(Shader& shader) override;
    void ShadowPassDraw(Shader& shader) override;
    void DrawGeometryPass(Shader& shader) override;

private:
    void buildPlane();

protected:
    void ApplyMaterialUniforms(Shader& shader) override;
    void DrawMesh() override;
};
