#include "Objects/Plane.h"

#include "DebugUtils.h"
#include "Lighting/Shader.h"
#include "Objects/Texture.h"

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
    //Bind Plane
    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Unbind buffers and reset state
    glBindVertexArray(0);
    glCheckError();
}

void Plane::buildPlane()
{

    //Pre Defined cube vertices, normals, and TexCoords
    float planeVertices[] = {
        // positions            // normals         // texcoords
         25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
        -25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
        -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,

         25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
        -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
         25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f
    };


    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    // unbind VAO and VBOs
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}