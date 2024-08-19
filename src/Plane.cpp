#include "Plane.h"

unsigned int Plane::planeCount = 0;


Plane::Plane(const char* texturePathDiffuse, const char* texturePathSpecular) : Object()
{
    //Set some rendering properties
    m_HasTexture = true;
    //Open and load diffuse map and specular map, save their Planes
    m_DiffuseMap = new Texture(texturePathDiffuse, false, "texture_diffuse");
    m_SpecularMap = new Texture(texturePathSpecular, false, "texture_specular");
    m_DisplayName = "Plane" + std::to_string(planeCount);
    m_ObjectID = planeCount;
    planeCount++;
    //Build the specified Plane type
    buildPlane();
}

//Used for creating a primitive with no texture
Plane::Plane() : Object()
{
    m_HasTexture = false;
    m_DisplayName = "Plane" + std::to_string(planeCount);
    m_ObjectID = planeCount;
    planeCount++;
    buildPlane();
}

void Plane::Draw(Shader& shader)
{
    shader.use();
    shader.setVec3("object.ambient", m_Ambient);
    shader.setVec3("object.diffuse", m_Diffuse);
    shader.setVec3("object.specular", m_Specular);
    shader.setMat4("model", m_Model);
    shader.setBool("hasTexture", m_HasTexture);

    //Bind texture and send texture to fragment shader
    if (m_HasTexture)
    {
        int diffuseUnit = TextureManager::getNextUnit();
        glActiveTexture(GL_TEXTURE0 + diffuseUnit); // activate the texture unit first before binding texture (2 texture in frag shader)
        glBindTexture(GL_TEXTURE_2D, m_DiffuseMap->ID);
        shader.setUInt("material.diffuse", diffuseUnit);

        int specularUnit = TextureManager::getNextUnit();
        glActiveTexture(GL_TEXTURE0 + specularUnit); // activate the texture unit first before binding texture (2 texture in frag shader)
        glBindTexture(GL_TEXTURE_2D, m_SpecularMap->ID);
        shader.setUInt("material.specular", specularUnit);
    }

    //Bind Plane
    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Unbind buffers and reset state
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glCheckError();
}

void Plane::ShadowPassDraw(Shader& shader)
{
    shader.use();
    shader.setMat4("model", m_Model);

    //Bind Plane and render
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
    glBindVertexArray(0);


    // unbind VAO and VBOs
    glBindVertexArray(0);
    //glBindBuffer(GL_ARRAY_BUFFER, 0);
}

int Plane::updateTexture(std::vector<std::string> texturePaths)
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
            dSuccess = m_DiffuseMap->updateTexture(texturePaths[0].c_str(), false);
            sSuccess = m_SpecularMap->updateTexture(texturePaths[1].c_str(), false);
        }

        if (dSuccess == 1 || sSuccess == 1)
        {
            //Textures failed to apply
            //As of right now, remove all textures if an invalid texture is applied
            delete m_DiffuseMap;
            delete m_SpecularMap;
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
            //Textures failed to apply
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


