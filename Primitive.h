#pragma once
#include <iostream>
#include <vector>

#include "Shader.h"
#include "Texture.h"
#include "Object.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Primitive : public Object
{
public:
    //Mesh Data
    std::vector<float>m_Vertices;
    std::vector<float>m_TexCoords;
    std::vector<float>m_Normals;
    std::vector<unsigned int>m_Indices;
    std::vector<float>m_InterleavedVertices;

    //Primitive Texture Attributes
    Texture* diffuseMap;
    Texture* specularMap;
    bool m_hasTexture;

    //Primitive Render Attributes
    unsigned int m_vao, m_vbo, m_ebo;
    bool m_useEBO;

    //Generate an Primitive with no mesh data, default color data, no texture data, default world attributes
    Primitive();
    //Build a primitive of type eg. type = "sphere", "cube", "pyramid"
    Primitive(const char* type, const char* texturePathDiffuse, const char* texturePathSpecular);
    //Build a primitive Primitive with no texture data
    Primitive(const char* type);

    //Methods
    void Draw(Shader& shader) override;

private:
    //Used for construction of primitives
    void buildInterleavedVertices();
    void buildInterleavedVerticesWithTexCoords();
    void buildSphere();
    void buildCube();



};