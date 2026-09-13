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
    int updateTexture(std::vector<std::string> texturePaths) override;


private:
    //Used for construction of primitives
    void buildSphere();

protected:
    void ApplyMaterialUniforms(Shader& shader) override;
    void DrawMesh() override;

};
