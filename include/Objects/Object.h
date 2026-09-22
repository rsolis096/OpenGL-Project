#pragma once
#include <vector>
#include <string>

#include <glm/glm.hpp>

#include "ObjectTypes.h"

class Shader;
class Texture;

//Holds some attributes that all "objects" inherit
//Does not contain rendering information as various objects render differently
class Object
{
public:

        static unsigned int objectCount;

        //Initializes the basic attributes for all objects
        Object();
        virtual ~Object();

        //Texture Properties
        Transform m_Transform;
        EntityInfo m_EntityInfo;
        Material m_Material;

        virtual void updateTexture(std::vector<std::string> texturePaths);

        //Draw the object. Each object type draws differently
        virtual void Draw(Shader& shader) = 0;
        virtual void ShadowPassDraw(Shader& shader) = 0;
        virtual void DrawGeometryPass(Shader& shader) = 0;
        virtual ObjectType GetType() const { return ObjectType::Object; }


    protected:

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

        virtual void ApplyMaterialUniforms(Shader& shader);
        virtual void DrawMesh();

};
