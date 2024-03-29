#pragma once

#include "Object.h"

class Plane : public Object
{
public:

    //Generate an Plane with no mesh data, default color data, no texture data, default world attributes
    Plane();
    Plane(const char* texturePathDiffuse, const char* texturePathSpecular);


    void Draw(Shader& shader) override;
    void ShadowMapDraw(Shader& shader) override;
    int updateTexture(std::vector<std::string> texturePaths) override;

private:
    void buildPlane();
    static unsigned int planeCount;
};