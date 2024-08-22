#pragma once

#include "Object.h"

class Cube : public Object
{
public:

    //Generate an Cube with no mesh data, default color data, no texture data, default world attributes
    Cube();
    ~Cube() override;
    //Build a primitive of type eg. type = "sphere", "cube", "pyramid"
    Cube(const char* texturePathDiffuse, const char* texturePathSpecular);

    //Methods
    void Draw(Shader& shader) override;
    void ShadowPassDraw(Shader& shader) override;
    ObjectType GetType() const override;


private:
    //Used for construction of primitives
    void buildCube();
    static unsigned int cubeCount;
    int updateTexture(std::vector<std::string> texturePaths) override;

};