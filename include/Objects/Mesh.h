#pragma once

#include "Objects/MeshData.h"

#include <string>
#include <vector>

class Shader;
using namespace std;

struct ModelTexture {
    unsigned int id;
    std::string type;
    std::string path;
};

class Mesh {
public:
    // mesh Data
    MeshData m_MeshData;
    vector<ModelTexture> textures;

    Mesh(MeshData meshData);
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<ModelTexture> textures);
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;
    ~Mesh();

    // render the mesh
    void Draw(Shader& shader, bool hasTexture) const;
    void ShadowPassDraw(Shader& shader) const;
    void DrawGeometryPass(Shader& shader, bool hasTexture) const;
    void DrawMesh() const;

private:
    // render data 
    unsigned int VAO = 0;
    unsigned int VBO = 0;
    unsigned int EBO = 0;

    // initializes all the buffer objects/arrays
    void setupMesh();
    void releaseGpuResources();

    void ApplyMaterialUniforms(Shader& shader, bool hasTexture) const;

};
