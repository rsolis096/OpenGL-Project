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

        //Object Texture Attributes
        Texture* diffuseMap;
        Texture* specularMap;
        Texture* basicTexture;
        bool m_hasTexture;

        //Object World Attributes
        glm::vec3 m_Position;
        glm::mat4 model;

        //Object Render Attributes
        unsigned int m_vao, m_vbo, m_ebo;
        Shader m_shader;
        bool m_useEBO;


        //Generate an object with no mesh data, default color data, no texture data, default world attributes
        Object();

        //Methods
        void render();

        //Teleport to specified location
        void setPosition(glm::vec3 newPosition);
        //Translate by parameter (Used to move some direction from current position)
        void translatePosition(glm::vec3 newPosition);
        void setAmbient(glm::vec3);
        void setDiffuse(glm::vec3);
        void setSpecular(glm::vec3);


    protected:
        void buildInterleavedVertices();
        void buildInterleavedVerticesWithTexCoords();



};