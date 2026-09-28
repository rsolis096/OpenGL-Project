#include "Objects/Plane.h"

#include "DebugUtils.h"
#include "Lighting/Shader.h"
#include "Objects/Mesh.h"
#include "Objects/Texture.h"

#include <cstddef>

Plane::Plane(const char* texturePathDiffuse, const char* texturePathSpecular) : Object()
{
    //Set some rendering properties
    m_Material.setTextures({ texturePathDiffuse , texturePathSpecular });

    //Build the specified Plane type
    buildPlane();
}

//Used for creating a primitive with no texture
Plane::Plane() : Object()
{
    buildPlane();
}

void Plane::Draw(Shader& shader)
{
    shader.use();
    shader.setMat4("model", m_Transform.matrix());

    ApplyMaterialUniforms(shader);
    DrawMesh();
}

void Plane::ShadowPassDraw(Shader& shader)
{
    shader.use();
    shader.setMat4("model", m_Transform.matrix());

    DrawMesh();
}

void Plane::DrawGeometryPass(Shader& shader)
{
    shader.use();
    shader.setMat4("model", m_Transform.matrix());

    ApplyMaterialUniforms(shader);
    DrawMesh();
}

void Plane::ApplyMaterialUniforms(Shader& shader)
{
    shader.use();

    shader.setMaterial(m_Material);

    //Bind texture and send texture to fragment shader
    if (m_Material.hasTextures())
    {
        glActiveTexture(GL_TEXTURE1); // activate the texture unit first before binding texture (2 texture in frag shader)
        glBindTexture(GL_TEXTURE_2D, m_Material.m_DiffuseMap->ID);

        glActiveTexture(GL_TEXTURE2); // activate the texture unit first before binding texture (2 texture in frag shader)
        glBindTexture(GL_TEXTURE_2D, m_Material.m_SpecularMap->ID);
    }
    glCheckError();
}

void Plane::DrawMesh()
{
    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_IndexCount), GL_UNSIGNED_INT, nullptr);

    glBindVertexArray(0);
    glCheckError();
}

void Plane::buildPlane()
{
    const MeshData meshData = buildPlaneMesh();
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
