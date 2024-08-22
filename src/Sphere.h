#pragma once

#include "Object.h"

class Sphere : public Object
{
public:



    bool changed;
    //Build a primitive Cube with no texture data
    Sphere();
    ~Sphere() override;
    //Build a primitive of type eg. type = "sphere", "cube", "pyramid"
    Sphere(const char* texturePathDiffuse, const char* texturePathSpecular);

    //Methods
    void Draw(Shader& shader) override;
    void ShadowPassDraw(Shader& shader) override;

    int updateTexture(std::vector<std::string> texturePaths) override;

private:
    //Used for construction of primitives
    void buildSphere();
    static unsigned int sphereCount;
};