#include "Cube.h"



//Used for creating a Primtive with texture information
Cube::Cube(const char* texturePathDiffuse, const char* texturePathSpecular) : Object()
{
    //Set some rendering properties
    m_hasTexture = true;
    m_Type = "Cube";
    //Open and load diffuse map and specular map, save their Cubes
    m_DiffuseMap = new Texture(texturePathDiffuse, false, "texture_diffuse");
    m_SpecularMap = new Texture(texturePathSpecular, false, "texture_specular");

    //Build the specified Cube type
    buildCube();
}

//Used for creating a primitive with no texture
Cube::Cube() : Object()
{
    m_hasTexture = false;
    m_Type = "Cube";
    buildCube();
}

void Cube::Draw(Shader& shader)
{
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

    //Bind Cube
    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Unbind buffers and reset state
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

}

void Cube::buildInterleavedVertices()
{
    std::vector<float>().swap(m_InterleavedVertices);

    std::size_t i, j;
    std::size_t count = m_Vertices.size();
    for (i = 0, j = 0; i < count; i += 3)
    {
        m_InterleavedVertices.push_back(m_Vertices[i]);
        m_InterleavedVertices.push_back(m_Vertices[i + 1]);
        m_InterleavedVertices.push_back(m_Vertices[i + 2]);


        m_InterleavedVertices.push_back(m_Normals[i]);
        m_InterleavedVertices.push_back(m_Normals[i + 1]);
        m_InterleavedVertices.push_back(m_Normals[i + 2]);

    }
}

void Cube::buildInterleavedVerticesWithTexCoords()
{
    std::vector<float>().swap(m_InterleavedVertices);

    std::size_t i, j;
    std::size_t count = m_Vertices.size();
    for (i = 0, j = 0; i < count; i += 3, j += 2)
    {
        m_InterleavedVertices.push_back(m_Vertices[i]);
        m_InterleavedVertices.push_back(m_Vertices[i + 1]);
        m_InterleavedVertices.push_back(m_Vertices[i + 2]);

        m_InterleavedVertices.push_back(m_Normals[i]);
        m_InterleavedVertices.push_back(m_Normals[i + 1]);
        m_InterleavedVertices.push_back(m_Normals[i + 2]);

        m_InterleavedVertices.push_back(m_TexCoords[j]);
        m_InterleavedVertices.push_back(m_TexCoords[j + 1]);
    }
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
    if (m_hasTexture == true)
        buildInterleavedVerticesWithTexCoords();
    else
        buildInterleavedVertices();

    //Generate VAO and VBO
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    //Assign vertices to Cube
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

