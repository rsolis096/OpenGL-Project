#include "Objects/Cube.h"

#include "DebugUtils.h"
#include "Lighting/Shader.h"
#include "Objects/Texture.h"

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
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glBindVertexArray(0);
    glCheckError();
}

void Cube::buildCube()
{
    //Pre Defined cube vertices, normals, and TexCoords
    m_Vertices = {
        // Back face
        -0.5f, -0.5f, -0.5f, // Bottom-left
         0.5f,  0.5f, -0.5f, // top-right
         0.5f, -0.5f, -0.5f,  // bottom-right         
         0.5f,  0.5f, -0.5f, // top-right
        -0.5f, -0.5f, -0.5f,   // bottom-left
        -0.5f,  0.5f, -0.5f,  // top-left
        // Front face
        -0.5f, -0.5f,  0.5f, // bottom-left
         0.5f, -0.5f,  0.5f,   // bottom-right
         0.5f,  0.5f,  0.5f,  // top-right
         0.5f,  0.5f,  0.5f,   // top-right
        -0.5f,  0.5f,  0.5f,   // top-left
        -0.5f, -0.5f,  0.5f,   // bottom-left
        // Left face
        -0.5f,  0.5f,  0.5f,   // top-right
        -0.5f,  0.5f, -0.5f,   // top-left
        -0.5f, -0.5f, -0.5f, // bottom-left
        -0.5f, -0.5f, -0.5f,   // bottom-left
        -0.5f, -0.5f,  0.5f, // bottom-right
        -0.5f,  0.5f,  0.5f,  // top-right
        // Right face
         0.5f,  0.5f,  0.5f, // top-left
         0.5f, -0.5f, -0.5f,   // bottom-right
         0.5f,  0.5f, -0.5f,   // top-right         
         0.5f, -0.5f, -0.5f,   // bottom-right
         0.5f,  0.5f,  0.5f,   // top-left
         0.5f, -0.5f,  0.5f, // bottom-left     
         // Bottom face
         -0.5f, -0.5f, -0.5f,   // top-right
          0.5f, -0.5f, -0.5f,   // top-left
          0.5f, -0.5f,  0.5f,  // bottom-left
          0.5f, -0.5f,  0.5f,   // bottom-left
         -0.5f, -0.5f,  0.5f, // bottom-right
         -0.5f, -0.5f, -0.5f,   // top-right
         // Top face
         -0.5f,  0.5f, -0.5f,  // top-left
          0.5f,  0.5f,  0.5f,  // bottom-right
          0.5f,  0.5f, -0.5f,   // top-right     
          0.5f,  0.5f,  0.5f,  // bottom-right
         -0.5f,  0.5f, -0.5f,   // top-left
         -0.5f,  0.5f,  0.5f  // bottom-left  
    };

    m_Normals = {
        0.0f,  0.0f, -1.0f,
        0.0f,  0.0f, -1.0f,
        0.0f,  0.0f, -1.0f,
        0.0f,  0.0f, -1.0f,
        0.0f,  0.0f, -1.0f,
        0.0f,  0.0f, -1.0f,

        0.0f,  0.0f,  1.0f,
        0.0f,  0.0f,  1.0f,
        0.0f,  0.0f,  1.0f,
        0.0f,  0.0f,  1.0f,
        0.0f,  0.0f,  1.0f,
        0.0f,  0.0f,  1.0f,

        -1.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,

         1.0f,  0.0f,  0.0f,
         1.0f,  0.0f,  0.0f,
         1.0f,  0.0f,  0.0f,
         1.0f,  0.0f,  0.0f,
         1.0f,  0.0f,  0.0f,
         1.0f,  0.0f,  0.0f,

         0.0f, -1.0f,  0.0f,
         0.0f, -1.0f,  0.0f,
         0.0f, -1.0f,  0.0f,
         0.0f, -1.0f,  0.0f,
         0.0f, -1.0f,  0.0f,
         0.0f, -1.0f,  0.0f,

         0.0f,  1.0f,  0.0f,
         0.0f,  1.0f,  0.0f,
         0.0f,  1.0f,  0.0f,
         0.0f,  1.0f,  0.0f,
         0.0f,  1.0f,  0.0f,
         0.0f,  1.0f,  0.0f
    };

    m_TexCoords = {
        // Back face
        0.0f, 0.0f, // Bottom-left
        1.0f, 1.0f, // top-right
        1.0f, 0.0f, // bottom-right         
        1.0f, 1.0f, // top-right
        0.0f, 0.0f, // bottom-left
        0.0f, 1.0f, // top-left
        // Front face
        0.0f, 0.0f, // bottom-left
        1.0f, 0.0f, // bottom-right
        1.0f, 1.0f, // top-right
        1.0f, 1.0f, // top-right
        0.0f, 1.0f, // top-left
        0.0f, 0.0f, // bottom-left
        // Left face
        1.0f, 0.0f, // top-right
        1.0f, 1.0f, // top-left
        0.0f, 1.0f, // bottom-left
        0.0f, 1.0f, // bottom-left
        0.0f, 0.0f, // bottom-right
        1.0f, 0.0f, // top-right
        // Right face
        1.0f, 0.0f, // top-left
        0.0f, 1.0f, // bottom-right
        1.0f, 1.0f, // top-right         
        0.0f, 1.0f, // bottom-right
        1.0f, 0.0f, // top-left
        0.0f, 0.0f, // bottom-left     
         // Bottom face
        0.0f, 1.0f, // top-right
        1.0f, 1.0f, // top-left
        1.0f, 0.0f, // bottom-left
        1.0f, 0.0f, // bottom-left
        0.0f, 0.0f, // bottom-right
        0.0f, 1.0f, // top-right
         // Top face
        0.0f, 1.0f, // top-left
        1.0f, 0.0f, // bottom-right
        1.0f, 1.0f, // top-right     
        1.0f, 0.0f, // bottom-right
        0.0f, 1.0f, // top-left
        0.0f, 0.0f  // bottom-left   
    };

    //Combine above mesh data
    buildInterleavedVerticesWithTexCoords();

    //Generate VAO and VBO
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    //Assign vertices to Cube
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_InterleavedVertices.size(), m_InterleavedVertices.data(), GL_STATIC_DRAW);

    // Set up vertex attribute pointers (vertices, normals, texcoords)
    glBindVertexArray(m_vao);
    //Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 8 * sizeof(float), (void*)0);
    //Normal Attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, false, 8 * sizeof(float), (void*)(sizeof(float) * 3));
    //TexCoord Attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, false, 8 * sizeof(float), (void*)(sizeof(float) * 6));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);


    // unbind VAO and VBOs
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}