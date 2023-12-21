#pragma once
#include <iostream>
#include <vector>

#include "Shader.h"
#include "Texture.h"
#include "Object.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class Plane : public Object
{
public:

    //Mesh Data
    std::vector<float>m_Vertices;
    std::vector<float>m_TexCoords;
    std::vector<float>m_Normals;
    std::vector<unsigned int>m_Indices;
    std::vector<float>m_InterleavedVertices;


    //Plane Render Attributes
    bool changed;
    //Generate an Plane with no mesh data, default color data, no texture data, default world attributes
    Plane();
    //Build a primitive of type eg. type = "sphere", "cube", "pyramid"
    Plane(const char* texturePathDiffuse, const char* texturePathSpecular);


    //Methods
    void Draw(Shader& shader) override;

private:
    //Used for construction of primitives
    void buildInterleavedVertices();
    void buildInterleavedVerticesWithTexCoords();
    void buildPlane();
};