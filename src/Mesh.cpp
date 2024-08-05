#include "Mesh.h"

Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<ModelTexture> textures)
{
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;

    setupMesh();
}

// render the mesh
void Mesh::Draw(Shader& shader, bool hasTexture, unsigned int textureUnitOffset)
{
    // bind appropriate textures
    unsigned int normalNr = 1;
    unsigned int heightNr = 1;

    if (hasTexture)
    {
        for (unsigned int i = 0; i < textures.size(); i++) {

            glActiveTexture(GL_TEXTURE0 + textureUnitOffset + i);  // Correct usage with GL_TEXTURE0 as base
            glBindTexture(GL_TEXTURE_2D, textures[i].id);

            std::string number;
            std::string name = textures[i].type;
            GLint location;
            if (name == "texture_diffuse") {
                location = glGetUniformLocation(shader.m_ProgramId, "material.diffuse");
            }
            else if (name == "texture_specular") {
                location = glGetUniformLocation(shader.m_ProgramId, "material.specular");
            }
            else if (name == "texture_normal") {
                number = std::to_string(normalNr++); // Convert unsigned int to string
                location = glGetUniformLocation(shader.m_ProgramId, ("material." + name + number).c_str());
            }
            else if (name == "texture_height") {
                number = std::to_string(heightNr++); // Convert unsigned int to string
                location = glGetUniformLocation(shader.m_ProgramId, ("material." + name + number).c_str());
            }
            else {
                continue;
            }

            glUniform1i(location, textureUnitOffset + i);
        }
    }

    // draw mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

    // always good practice to set everything back to defaults once configured.
    glActiveTexture(GL_TEXTURE0);
    glCheckError();
}

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
