#include "SkyBox.h"

//Pre Defined cube maps for sky boxes
std::vector<std::vector<std::string>> SkyBox::m_CubeMapPaths = {

    {
        "assets/skybox/city/right.jpg",
        "assets/skybox/city/left.jpg",
        "assets/skybox/city/top.jpg",
        "assets/skybox/city/bottom.jpg",
        "assets/skybox/city/front.jpg",
        "assets/skybox/city/back.jpg"
    }
    ,
    {
        "assets/skybox/water/right.jpg",
        "assets/skybox/water/left.jpg",
        "assets/skybox/water/top.jpg",
        "assets/skybox/water/bottom.jpg",
        "assets/skybox/water/front.jpg",
        "assets/skybox/water/back.jpg"
    }
    ,
    {
        "assets/skybox/mountain/right.jpg",
        "assets/skybox/mountain/left.jpg",
        "assets/skybox/mountain/top.jpg",
        "assets/skybox/mountain/bottom.jpg",
        "assets/skybox/mountain/front.jpg",
        "assets/skybox/mountain/back.jpg"
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
    m_CubeMap = new Texture(m_CubeMapPaths[1], m_InvertedTexture);
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