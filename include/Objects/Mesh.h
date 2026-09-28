#pragma once

#define MAX_BONE_INFLUENCE 4

#include <glm/glm.hpp>
#include <string>
#include <vector>

class Shader;
using namespace std;

struct ModelTexture {
    unsigned int id;
    std::string type;
    std::string path;
};

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;

    //bone indexes which will influence this vertex
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    //weights from each bone
    float m_Weights[MAX_BONE_INFLUENCE];
};

struct MeshData {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
};

MeshData buildCubeMesh();
MeshData buildSphereMesh();
MeshData buildPlaneMesh();

class Mesh {
public:
    // mesh Data
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<ModelTexture> textures;

    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<ModelTexture> textures);
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    Mesh(Mesh&& other);
    Mesh& operator=(Mesh&& other) ;
    ~Mesh();

    // render the mesh
    void Draw(Shader& shader, bool hasTexture) const;
    void ShadowPassDraw(Shader& shader) const;
    void DrawGeometryPass(Shader& shader, bool hasTexture) const;

private:
    // render data 
    unsigned int VAO = 0;
    unsigned int VBO = 0;
    unsigned int EBO = 0;
    // initializes all the buffer objects/arrays
    void setupMesh();
    void releaseGpuResources();

    void ApplyMaterialUniforms(Shader& shader, bool hasTexture) const;
    void DrawMesh() const;

};
