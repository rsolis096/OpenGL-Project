#include "Mesh.h"

Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<ModelTexture> textures)
{
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;

    setupMesh();
}

// Move constructor
Mesh::Mesh(Mesh&& other) noexcept
    : vertices(std::move(other.vertices)),
    indices(std::move(other.indices)),
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
        vertices = std::move(other.vertices);
        indices = std::move(other.indices);
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
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    vertices.clear();
    vertices.shrink_to_fit();
    indices.clear();
    indices.shrink_to_fit();
    textures.clear();
    textures.shrink_to_fit();
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
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
    glCheckError();
}

// From LearnOpenGL
void Mesh::setupMesh()
{
    // create buffers/arrays
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    // load data into vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // A great thing about structs is that their memory layout is sequential for all its items.
    // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
    // again translates to 3/2 floats which translates to a byte array.
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // set the vertex attribute pointers
    // vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    // vertex tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
    // vertex bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
    // ids
    glEnableVertexAttribArray(5);
    glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));
    // weights
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));
    glBindVertexArray(0);
}
