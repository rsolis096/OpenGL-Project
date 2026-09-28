#include "Objects/Mesh.h"

#include "DebugUtils.h"
#include "Lighting/Shader.h"

#include <cstddef>
#include <utility>

Mesh::Mesh(MeshData meshData) :
    m_MeshData(std::move(meshData))
{
    setupMesh();
}

Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<ModelTexture> textures)
{
    m_MeshData.vertices = vertices;
    m_MeshData.indices = indices;
    this->textures = textures;

    setupMesh();
}

// Move constructor
Mesh::Mesh(Mesh&& other) noexcept :
    m_MeshData(std::move(other.m_MeshData)),
    textures(std::move(other.textures)),
    VAO(other.VAO),
    VBO(other.VBO),
    EBO(other.EBO)
{
    other.VAO = 0;
    other.VBO = 0;
    other.EBO = 0;
}


// Move assignment operator
Mesh& Mesh::operator=(Mesh&& other) noexcept
{
    std::cout << "Move assignment operator called\n";
    if (this != &other)
    {
        releaseGpuResources();
        m_MeshData = std::move(other.m_MeshData);
        textures = std::move(other.textures);
        VAO = other.VAO;
        VBO = other.VBO;
        EBO = other.EBO;

        other.VAO = 0;
        other.VBO = 0;
        other.EBO = 0;
    }
    return *this;
}

Mesh::~Mesh()
{
    //std::cout << "Mesh Destructor called\n";
    releaseGpuResources();

    textures.clear();
    textures.shrink_to_fit();
}

void Mesh::releaseGpuResources()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    VAO = 0;
    VBO = 0;
    EBO = 0;
}

void Mesh::ShadowPassDraw(Shader& shader) const
{
    DrawMesh();
}

void Mesh::DrawGeometryPass(Shader& shader, bool hasTexture) const
{
    ApplyMaterialUniforms(shader, hasTexture);
    DrawMesh();
}

// render the mesh
void Mesh::Draw(Shader& shader, bool hasTexture) const
{
    ApplyMaterialUniforms(shader, hasTexture);
    DrawMesh();
}

void Mesh::ApplyMaterialUniforms(Shader& shader, bool hasTexture) const
{
    shader.use();
    shader.setBool("hasTexture", hasTexture);

    // bind appropriate textures
    unsigned int normalNr = 1;
    unsigned int heightNr = 1;

    bool hasBindedDiffuse = false;
    bool hasBindedSpecular = false;
    bool hasBindedNormal = false;

    if (hasTexture)
    {
        for (unsigned int i = 0; i < textures.size(); i++)
        {
            std::string number;
            std::string name = textures[i].type;
            GLint location;
            if (name == "texture_diffuse") {
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, textures[i].id);
                location = glGetUniformLocation(shader.m_ProgramId, "material.diffuse");
                glUniform1i(location, 1);
            }
            else if (name == "texture_specular") {
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, textures[i].id);
                location = glGetUniformLocation(shader.m_ProgramId, "material.specular");
                glUniform1i(location, 2);
            }
            else if (name == "texture_normal") {
                glActiveTexture(GL_TEXTURE3);
                glBindTexture(GL_TEXTURE_2D, textures[i].id);
                number = std::to_string(normalNr++); // Convert unsigned int to string
                location = glGetUniformLocation(shader.m_ProgramId, ("material." + name + number).c_str());
                glUniform1i(location, 3);
            }
            /*
            else if (name == "texture_height") {
                glActiveTexture(GL_TEXTURE10);
                glBindTexture(GL_TEXTURE_2D, textures[i].id);
                number = std::to_string(heightNr++); // Convert unsigned int to string
                location = glGetUniformLocation(shader.m_ProgramId, ("material." + name + number).c_str());
                glUniform1i(location, 10);
            }
            else {
                std::cout << "continued \n";
                continue;
            }
            */
        }
    }
}

void Mesh::DrawMesh() const
{
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(m_MeshData.indices.size()), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
    glCheckError();
}

// From LearnOpenGL
void Mesh::setupMesh()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        m_MeshData.vertices.size() * sizeof(Vertex),
        m_MeshData.vertices.data(),
        GL_STATIC_DRAW
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        m_MeshData.indices.size() * sizeof(unsigned int),
        m_MeshData.indices.data(),
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, Normal)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, TexCoords)));
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, Tangent)));
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, Bitangent)));
    glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, m_BoneIDs)));
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, m_Weights)));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);
    glEnableVertexAttribArray(5);
    glEnableVertexAttribArray(6);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
