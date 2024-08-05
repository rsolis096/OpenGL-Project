#include "SkyBox.h"

//Pre Defined cube maps for sky boxes
std::vector<std::vector<std::string>> SkyBox::m_CubeMapPaths = {

    {
        "Assets/skybox/city/right.jpg",
        "Assets/skybox/city/left.jpg",
        "Assets/skybox/city/top.jpg",
        "Assets/skybox/city/bottom.jpg",
        "Assets/skybox/city/front.jpg",
        "Assets/skybox/city/back.jpg"
    }
    ,
    {
        "Assets/skybox/water/right.jpg",
        "Assets/skybox/water/left.jpg",
        "Assets/skybox/water/top.jpg",
        "Assets/skybox/water/bottom.jpg",
        "Assets/skybox/water/front.jpg",
        "Assets/skybox/water/back.jpg"
    }
    ,
    {
        "Assets/skybox/mountain/right.jpg",
        "Assets/skybox/mountain/left.jpg",
        "Assets/skybox/mountain/top.jpg",
        "Assets/skybox/mountain/bottom.jpg",
        "Assets/skybox/mountain/front.jpg",
        "Assets/skybox/mountain/back.jpg"
    }
};

std::vector<std::string> SkyBox::m_CubeMapNames = {
    "city",
    "water",
    "mountain",
};

SkyBox::SkyBox(Shader& s, Camera* c) : m_CubeMapShader(s), m_PlayerCamera(c)
{
    //Skybox faces
    m_Vertices = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    glGenVertexArrays(1, &m_SkyBoxVAO);
    glGenBuffers(1, &m_SkyBoxVBO);
    glBindVertexArray(m_SkyBoxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_SkyBoxVBO);
    glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(float), m_Vertices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glCheckError();
    m_InvertedTexture = false;
    m_CubeMap = new Texture(m_CubeMapPaths[0], m_InvertedTexture);
    m_Current = 0;
}

SkyBox::~SkyBox()
{
    //Delete all objects
    glDeleteVertexArrays(1, &m_SkyBoxVAO);
    glDeleteBuffers(1, &m_SkyBoxVBO);
}

void SkyBox::draw(glm::mat4& projection)
{
    glDepthFunc(GL_LEQUAL);  // Change depth function so depth test passes when values are equal to depth buffer's content
    m_CubeMapShader.use();
    glm::mat4 view = glm::mat4(glm::mat3(m_PlayerCamera->GetViewMatrix())); // Remove translation from the view matrix
    m_CubeMapShader.setMat4("view", view);
    m_CubeMapShader.setMat4("projection", projection);

    // Bind the correct texture unit
    GLuint textureUnit = TextureManager::getNextUnit();
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_CubeMap->ID);

    // Update the shader with the new texture unit
    GLuint skyboxLoc = glGetUniformLocation(m_CubeMapShader.m_ProgramId, "skybox");
    glUniform1i(skyboxLoc, textureUnit);

    // Draw the skybox
    glBindVertexArray(m_SkyBoxVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    glDepthFunc(GL_LESS); // Set depth function back to default
    glCheckError();
}

int SkyBox::setCubeMapTexture(int i)
{
    // Ensure the index is within the valid range
    if (i >= 0 && i < m_CubeMapNames.size())
    {
        // Delete the current texture if it exists
        if (m_CubeMap)
        {
            delete m_CubeMap;
            m_CubeMap = nullptr;
        }

        // Load the new texture
        m_CubeMap = new Texture(m_CubeMapPaths[i], m_InvertedTexture);
        if (!m_CubeMap)
        {
            std::cerr << "Failed to load cube map texture\n";
            return 0; // Indicate failure
        }

        // Update the current texture index
        m_Current = i;

        return 1; // Indicate success
    }
    return 0; // No change needed
}