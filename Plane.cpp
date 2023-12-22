#include "Plane.h"

//Used for creating a Primtive with texture information
Plane::Plane(const char* texturePathDiffuse, const char* texturePathSpecular) : Object()
{
    //Set some rendering properties
    m_hasTexture = true;
    m_Type = "Plane";
    //Open and load diffuse map and specular map, save their Planes
    m_DiffuseMap = new Texture(texturePathDiffuse, false, "texture_diffuse");
    m_SpecularMap = new Texture(texturePathSpecular, false, "texture_specular");

    //Build the specified Plane type
    buildPlane();
}

//Used for creating a primitive with no texture
Plane::Plane() : Object()
{
    m_hasTexture = false;
    m_Type = "Plane";
    buildPlane();
}

void Plane::Draw(Shader& shader)
{
    //glDisable(GL_CULL_FACE);
    shader.use();
    shader.setVec3("object.ambient", m_Ambient);
    shader.setVec3("object.diffuse", m_Diffuse);
    shader.setVec3("object.specular", m_Specular);
    shader.setMat4("model", m_Model);

    //Bind texture and send texture to fragment shader
    if (m_hasTexture)
    {
        glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture (2 texture in frag shader)
        glBindTexture(GL_TEXTURE_2D, m_DiffuseMap->ID);
        glActiveTexture(GL_TEXTURE1); // activate the texture unit first before binding texture (2 texture in frag shader)
        glBindTexture(GL_TEXTURE_2D, m_SpecularMap->ID);

        shader.use();
        shader.setBool("hasTexture", true);
    }
    else
    {
        shader.use();
        shader.setBool("hasTexture", false);
    }

    //Bind Plane
    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Unbind buffers and reset state
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    //glEnable(GL_CULL_FACE);//Disable then re-enable to show both sides of plane
}

void Plane::buildPlane()
{
    //Pre Defined cube vertices, normals, and TexCoords
    m_Vertices = {

        // Top face
        -0.5f,  0.0f, -0.5f,  // top-left
        0.5f,  0.0f,  0.5f,  // bottom-right
        0.5f,  0.0f, -0.5f,   // top-right     
        0.5f,  0.0f,  0.5f,  // bottom-right
        -0.5f,  0.0f, -0.5f,   // top-left
        -0.5f,  0.0f,  0.5f  // bottom-left  
    };

    m_Normals = {

         0.0f,  1.0f,  0.0f,
         0.0f,  1.0f,  0.0f,
         0.0f,  1.0f,  0.0f,
         0.0f,  1.0f,  0.0f,
         0.0f,  1.0f,  0.0f,
         0.0f,  1.0f,  0.0f
    };

    m_TexCoords = {

       // Top face
        0.0f, 1.0f, // top-left
        1.0f, 0.0f, // bottom-right
        1.0f, 1.0f, // top-right     
        1.0f, 0.0f, // bottom-right
        0.0f, 1.0f, // top-left
        0.0f, 0.0f  // bottom-left   
    };

    //Combine above mesh data
    if (m_hasTexture == true)
        buildInterleavedVerticesWithTexCoords();
    else
        buildInterleavedVertices();

    //Generate VAO and VBO
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    //Assign vertices to Plane
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_InterleavedVertices.size(), m_InterleavedVertices.data(), GL_STATIC_DRAW);

    // Set up vertex attribute pointers (vertices, normals, texcoords)
    if (m_hasTexture)
    {
        //Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, false, 8 * sizeof(float), (void*)0);
        //Normal Attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, false, 8 * sizeof(float), (void*)(sizeof(float) * 3));
        //TexCoord Attribute
        glVertexAttribPointer(2, 2, GL_FLOAT, false, 8 * sizeof(float), (void*)(sizeof(float) * 6));

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
    }
    else
    {
        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        //Normal Attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
    }

    // unbind VAO and VBOs
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

