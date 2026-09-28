#include "Objects/Sphere.h"

#include "DebugUtils.h"
#include "Lighting/Shader.h"
#include "Objects/Mesh.h"
#include "Objects/Texture.h"

#include <cstddef>

//Used for creating a Primitive with texture information
Sphere::Sphere(const char* texturePathDiffuse, const char* texturePathSpecular) : Object()
{
    // Initialize the unique_ptrs using std::make_unique
    m_Material.setTextures({ texturePathDiffuse , texturePathSpecular });

    // Build the specified Sphere type
    buildSphere();
}

//Used for creating a primitive with no texture
Sphere::Sphere() : Object()
{
    buildSphere();
}

void Sphere::Draw(Shader& shader)
{
    shader.use();
    shader.setMat4("model", m_Transform.matrix());

    ApplyMaterialUniforms(shader);
    DrawMesh();
}

void Sphere::ShadowPassDraw(Shader& shader)
{
    shader.use();
    shader.setMat4("model", m_Transform.matrix());

    DrawMesh();
}

void Sphere::DrawGeometryPass(Shader& shader)
{
    shader.use();
    shader.setMat4("model", m_Transform.matrix());

    ApplyMaterialUniforms(shader);
    DrawMesh();
}

void Sphere::ApplyMaterialUniforms(Shader& shader)
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

void Sphere::DrawMesh()
{
    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_IndexCount), GL_UNSIGNED_INT, nullptr);

    glBindVertexArray(0);

    glCheckError();
}

void Sphere::buildSphere()
{
    const MeshData meshData = buildSphereMesh();
    m_IndexCount = static_cast<unsigned int>(meshData.indices.size());

    //Setup VAO and VBO
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

    // Bind and fill EBO with indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sizeof(unsigned int) * meshData.indices.size(),
        meshData.indices.data(),
        GL_STATIC_DRAW);

    // Set up vertex attribute pointers (vertices, normals, texcoords)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr); // position
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, Normal)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, TexCoords)));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    // unbind VAO and VBOs
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
