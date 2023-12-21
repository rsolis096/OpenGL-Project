#pragma once
#include <iostream>
#include <vector>

#include "Shader.h"
#include "Texture.h"
#include "Object.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class Sphere : public Object
{
public:

    //Mesh Data
    std::vector<float>m_Vertices;
    std::vector<float>m_TexCoords;
    std::vector<float>m_Normals;
    std::vector<unsigned int>m_Indices;
    std::vector<float>m_InterleavedVertices;

    bool changed;
    //Build a primitive Cube with no texture data
    Sphere();
    //Build a primitive of type eg. type = "sphere", "cube", "pyramid"
    Sphere(const char* texturePathDiffuse, const char* texturePathSpecular);

    //Methods
    void Draw(Shader& shader) override;

private:
    //Used for construction of primitives
    void buildInterleavedVertices();
    void buildInterleavedVerticesWithTexCoords();
    void buildSphere();
};