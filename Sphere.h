#pragma once
#include <iostream>
#include <vector>

#include "Object.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class Sphere : public Object
{
public:



    bool changed;
    //Build a primitive Cube with no texture data
    Sphere();
    //Build a primitive of type eg. type = "sphere", "cube", "pyramid"
    Sphere(const char* texturePathDiffuse, const char* texturePathSpecular);

    //Methods
    void Draw(Shader& shader) override;
    void updateTexture(std::vector<std::string> texturePaths) override {}

private:
    //Used for construction of primitives
    void buildSphere();
    static unsigned int sphereCount;
};