#pragma once
#include <iostream>
#include <vector>
#include <string>

#include "Shader.h"
#include "Texture.h"


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h> //Window functions

//Holds some attributes that all "objects" inherit
//Does not contain rendering information as various objects render differently
class Object
{
public:
        //Initializes the basic attributes for all objects
        Object();
        virtual ~Object();

        //Texture Properties
        Texture* m_DiffuseMap;
        Texture* m_SpecularMap;
        bool m_HasTexture;

        //Object World Attributes
        glm::vec3 m_Position;
        glm::vec3 m_Scale;
        glm::mat4 m_Model;
        glm::vec3 m_Rotation; //in degrees
        //glm::vec3 m_RotationAxis;
        //int m_RotationIndex;


        //Physics info (push to component later)
        glm::vec3 m_Force;
        glm::vec3 m_Velocity;
        glm::vec3 m_Mass;
        float startFall;
        bool enablePhysics;
        bool isPhysicsObject;
        void setPhysics();
        void setVelocity(glm::vec3);

        //Type
        unsigned int m_ObjectID;
        std::string m_DisplayName;


        //Teleport to specified location 
        void setPosition(glm::vec3 newPosition);                // Teleport object to a different position
        void setScale(glm::vec3 newScale);                      // Change scale of object in 3 dimensions
        void translatePosition(glm::vec3 newPosition);          // Translate by parameter (Used to move some diistance from current position)
        void updateObject();
        void setRotation(glm::vec3);

        //Change these properties to set color of object
        void setAmbient(glm::vec3);
        void setDiffuse(glm::vec3);
        void setSpecular(glm::vec3);

        glm::vec3 getAmbient() const;
        glm::vec3 getDiffuse() const;
        glm::vec3 getSpecular() const;

        //Draw the object. Each object type draws differently
        virtual void Draw(Shader& shader) = 0;
        virtual void ShadowPassDraw(Shader& shader) = 0;
        virtual int updateTexture(std::vector<std::string> texturePaths) = 0;


    protected:

        //Object color attributes
        glm::vec3 m_Ambient;
        glm::vec3 m_Diffuse;
        glm::vec3 m_Specular;

        //Mesh Data
        std::vector<float>m_Vertices;
        std::vector<float>m_TexCoords;
        std::vector<float>m_Normals;
        std::vector<unsigned int>m_Indices;
        std::vector<float>m_InterleavedVertices;

        //Rendering info
        unsigned int m_vao, m_vbo, m_ebo;

        void buildInterleavedVerticesWithTexCoords();
        void buildInterleavedVertices();
        

};