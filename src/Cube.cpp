#include "Cube.h"

#include <glm/gtx/string_cast.hpp>

unsigned int Cube::cubeCount = 0;

Cube::ObjectType Cube::GetType() const { return ObjectType::Cube; }

//Used for creating a Primtive with texture information
Cube::Cube(const char* texturePathDiffuse, const char* texturePathSpecular) : Object()
{
    //Set some rendering properties
    m_HasTexture = true;
    //Open and load diffuse map and specular map, save their Cubes
    m_DiffuseMap = new Texture(texturePathDiffuse, false, "material.diffuse");
    m_SpecularMap = new Texture(texturePathSpecular, false, "material.diffuse");
    m_DisplayName = "Cube" + std::to_string(cubeCount);
    m_ObjectID = cubeCount;
    cubeCount++;
    //Build the specified Cube type
    buildCube();
}

//Used for creating a primitive with no texture
Cube::Cube() : Object()
{
    m_HasTexture = false;
    m_DisplayName = "Cube" + std::to_string(cubeCount);
    m_ObjectID = cubeCount;
    buildCube();
    cubeCount++;
}

Cube::~Cube()
{
    std::cout << "Cube Deleted\n";
    cubeCount -= 1;
}

void Cube::Draw(Shader& shader)
{
    shader.use();
    shader.setMat4("model", m_Model);
    shader.setVec3("object.ambient", m_Ambient);
    shader.setVec3("object.diffuse", m_Diffuse);
    shader.setVec3("object.specular", m_Specular);
    shader.setBool("hasTexture", m_HasTexture);

    //Bind texture and send texture to fragment shader
    if (m_HasTexture)
    {

        glActiveTexture(GL_TEXTURE0 + TextureManager::getNextUnit()); // activate the texture unit first before binding texture (2 texture in frag shader)
        glBindTexture(GL_TEXTURE_2D, m_DiffuseMap->ID);
        shader.setUInt("material.diffuse", TextureManager::getCurrentUnit());

        glActiveTexture(GL_TEXTURE0 + TextureManager::getNextUnit()); // activate the texture unit first before binding texture (2 texture in frag shader)
        glBindTexture(GL_TEXTURE_2D, m_SpecularMap->ID);
        shader.setUInt("material.specular", TextureManager::getCurrentUnit());

    }
    
    //Bind Cube
    glBindVertexArray(m_vao);
    //Render
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Unbind buffers and reset state
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glCheckError();
}

void Cube::ShadowPassDraw(Shader& shader)
{
    shader.use();
    shader.setMat4("model", m_Model);

    //Bind Cube and render
    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Unbind buffers and reset state
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
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

int Cube::updateTexture(std::vector<std::string> texturePaths)
{
    // Three Scenarios
    // 1. Updating a texture of an Object that already has textures
    // 2. Updating a texture of an Object with no initial texture
    // 3. Updating the texture of a model object

    //Secenario 1
    if (m_HasTexture == true)
    {
        //Success flags
        int dSuccess = 1;
        int sSuccess = 1;

        if (texturePaths.size() == 2)
        {
            //If vector contains two strings, attempt to load their textures
            dSuccess = m_DiffuseMap->updateTexture(texturePaths[0].c_str(), false);
            sSuccess = m_SpecularMap->updateTexture(texturePaths[1].c_str(), false);
        }

        if (dSuccess == 1 || sSuccess == 1)
        {
            //If 1 or the other texture from the previous if statements failes to load,
            //delete the current textures and set the object to have no texture
            delete m_DiffuseMap;
            delete m_SpecularMap;
            m_DiffuseMap = nullptr;
            m_SpecularMap = nullptr;
            m_HasTexture = false;
            return 0;
        }
        else
            std::cout << "Updated Texture successfully!" << std::endl;
        return 1;
    }
    //Scenario 2
    else if (m_HasTexture == false)
    {
        m_DiffuseMap = new Texture(texturePaths[0].c_str(), false, "texture_diffuse");
        m_SpecularMap = new Texture(texturePaths[1].c_str(), false, "texture_specular");
        if (m_DiffuseMap->ID == GL_INVALID_VALUE || m_SpecularMap->ID == GL_INVALID_VALUE)
        {
            delete m_DiffuseMap;
            delete m_SpecularMap;
            m_DiffuseMap = nullptr;
            m_SpecularMap = nullptr;
            m_HasTexture = false;
            return 0;
        }
        else
            m_HasTexture = true;
        return 1;
    }
    return 0;
}


