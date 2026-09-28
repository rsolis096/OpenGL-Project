#include "Objects/Cube.h"

#include "DebugUtils.h"
#include "Lighting/Shader.h"
#include "Objects/MeshBuilder.h"
#include "Objects/Texture.h"

#include <cstddef>
#include <glm/gtx/string_cast.hpp>

//Used for creating a Primtive with texture information
Cube::Cube(const char* texturePathDiffuse, const char* texturePathSpecular) : Object()
{
    //Open and load diffuse map and specular map, save their Cubes
    m_Material.setTextures({ texturePathDiffuse , texturePathSpecular });

    //Build the specified Cube type
    buildCube();
}

//Used for creating a primitive with no texture
Cube::Cube() : Object()
{
    buildCube();
}

void Cube::Draw(Shader& shader)
{
    shader.use();
    shader.setMat4("model", m_Transform.matrix());

    ApplyMaterialUniforms(shader);
    DrawMesh();
}

void Cube::ShadowPassDraw(Shader& shader)
{
    shader.use();
    shader.setMat4("model", m_Transform.matrix());
    DrawMesh();
}

void Cube::DrawGeometryPass(Shader& shader)
{
    shader.use();
    shader.setMat4("model", m_Transform.matrix());

    ApplyMaterialUniforms(shader);
    DrawMesh();
}

void Cube::ApplyMaterialUniforms(Shader& shader)
{
    shader.use();

    shader.setMaterial(m_Material);


    if (m_Material.hasTextures())
    {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_Material.m_DiffuseMap->ID);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, m_Material.m_SpecularMap->ID);
    }
}

void Cube::DrawMesh()
{
    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_IndexCount), GL_UNSIGNED_INT, nullptr);

    glBindVertexArray(0);
    glCheckError();
}

void Cube::buildCube()
{
    const MeshData meshData = MeshBuilder::build(PrimitiveKind::Cube);
    m_IndexCount = static_cast<unsigned int>(meshData.indices.size());

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(Vertex) * meshData.vertices.size(),
        meshData.vertices.data(),
        GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sizeof(unsigned int) * meshData.indices.size(),
        meshData.indices.data(),
        GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, Normal)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, TexCoords)));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);


    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
