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

private:
    //Used for construction of primitives
    void buildCube();
    int updateTexture(std::vector<std::string> texturePaths) override;

protected:
    void ApplyMaterialUniforms(Shader& shader) override;
    void DrawMesh() override;

};
