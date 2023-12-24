#pragma once
#include <iostream>
#include <vector>

#include "Object.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class Cube : public Object
{
public:

    //Generate an Cube with no mesh data, default color data, no texture data, default world attributes
    Cube();
    //Build a primitive of type eg. type = "sphere", "cube", "pyramid"
    Cube(const char* texturePathDiffuse, const char* texturePathSpecular);

    //Methods
    void Draw(Shader& shader) override;

private:
    //Used for construction of primitives
    void buildCube();
    static unsigned int cubeCount;
};