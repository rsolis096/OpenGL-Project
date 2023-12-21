#pragma once

#include <iostream>
#include <vector>

#include "Shader.h"
#include "Texture.h"
#include "Vertex.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


//Holds some attributes that all "objects" inherit
//Does not contain rendering information as various objects render differently
struct Object
{
        //Initializes the basic attributes for all objects
        Object();

        //Object color attributes (ONLY FOR PRIMITIVES/ No texture Objects)
        glm::vec3 m_Ambient;
        glm::vec3 m_Diffuse;
        glm::vec3 m_Specular;

        //Texture Properties
        Texture* m_DiffuseMap;
        Texture* m_SpecularMap;
        bool m_hasTexture;

        //Object World Attributes
        glm::vec3 m_Position;
        glm::vec3 m_Scale;
        glm::mat4 m_Model;

        //Type
        std::string m_Type;

        //Rendering info
        unsigned int m_vao, m_vbo, m_ebo;

        //Teleport to specified location
        void setPosition(glm::vec3 newPosition);
        void setScale(glm::vec3 newScale);
        //Translate by parameter (Used to move some direction from current position)
        void translatePosition(glm::vec3 newPosition);

        //Change these properties to set color of object
        void setAmbient(glm::vec3);
        void setDiffuse(glm::vec3);
        void setSpecular(glm::vec3);

        virtual void Draw(Shader& shader) = 0;

    private:
        void updateModel();
};