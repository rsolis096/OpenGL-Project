#pragma once

#include "Shader.h"
#include "Texture.h"
#define MAX_BONE_INFLUENCE 4

#include <string>
#include <vector>
using namespace std;

struct ModelTexture {
    unsigned int id;
    std::string type;
    std::string path;
};

struct Vertex {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
    // tangent
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;
    //bone indexes which will influence this vertex
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    //weights from each bone
    float m_Weights[MAX_BONE_INFLUENCE];
};

class Mesh {
public:
    // mesh Data
    vector<Vertex>       vertices;
    vector<unsigned int> indices;
    vector<ModelTexture>      textures;

    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<ModelTexture> textures);
    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;
    ~Mesh();
    // render the mesh
    void Draw(Shader& shader, bool hasTexture, GLuint textureUnitOffset) const;
    void ShadowPassDraw(Shader& shader) const;
private:
    // render data 
    unsigned int VAO, VBO, EBO;
    // initializes all the buffer objects/arrays
    void setupMesh();
};
