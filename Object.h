#pragma once
#include <iostream>
#include <vector>

#include "Shader.h"
#include "Texture.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Object
{
    public: 
        //Mesh Data
        std::vector<float>m_Vertices;
        std::vector<float>m_TexCoords;
        std::vector<float>m_Normals;
        std::vector<unsigned int>m_Indices;
        std::vector<float>m_InterleavedVertices;

        //Object color attributes
        glm::vec3 m_Ambient;
        glm::vec3 m_Diffuse;
        glm::vec3 m_Specular;

        //Other Attributes
        Texture* diffuseMap;
        Texture* specularMap;
        Texture* basicTexture;

        unsigned int m_vao, m_vbo, m_ebo;
        unsigned int m_shaderID;
        bool m_hasTexture;
        bool m_useEBO;
        glm::vec3 m_Position;

        //Generate an object with texture data but no mesh data
        Object(unsigned int sId, const char* texturePathDiffuse, const char* texturePathSpecular);
        //Generate an object with no texture data
        Object(unsigned int sId);
        void render();

    protected:
        void buildInterleavedVertices();
        void buildInterleavedVerticesWithTexCoords();


};