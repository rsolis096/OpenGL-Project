#pragma once

#include <iostream>
#include <vector>

#include "Shader.h"
#include "Texture.h"
#include "Vertex.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//This class serves exclusively as a way to organize object colors and their transformations

struct Object
{
    public: 
        //Object color attributes (ONLY FOR PRIMITIVES/ No texture Objects)
        glm::vec3 m_Ambient;
        glm::vec3 m_Diffuse;
        glm::vec3 m_Specular;

        //Object World Attributes
        glm::vec3 m_Position;
        glm::mat4 model;

        //Teleport to specified location
        void setPosition(glm::vec3 newPosition);
        //Translate by parameter (Used to move some direction from current position)
        void translatePosition(glm::vec3 newPosition);
        void setAmbient(glm::vec3);
        void setDiffuse(glm::vec3);
        void setSpecular(glm::vec3);
};