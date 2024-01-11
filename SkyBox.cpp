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
    "mountain"
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

    m_CubeMap = new Texture(m_CubeMapPaths[0]);
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
    // draw skybox as last
    glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
    m_CubeMapShader.use();
    glm::mat4 view = glm::mat4(glm::mat3(m_PlayerCamera->GetViewMatrix())); // remove translation from the view matrix
    m_CubeMapShader.setMat4("view", view);
    m_CubeMapShader.setMat4("projection", projection);
    // skybox cube
    glBindVertexArray(m_SkyBoxVAO);
    //glActiveTexture(GL_TEXTURE0 + TextureManager::getNextUnit());
    glActiveTexture(GL_TEXTURE0); //BUG THIS TEXTURE UNIT MUST BE 0, FIX THIS
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_CubeMap->ID);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS); // set depth function back to default
    glCheckError();
}

int SkyBox::setCubeMapTexture(int i)
{
    if ((i <= 0 || i < m_CubeMapNames.size()) && m_Current != i)
    {
        std::cout << " 0 < " << i << " <" << m_CubeMapNames.size() << std::endl;
        delete m_CubeMap;
        m_CubeMap = nullptr;
        m_CubeMap = new Texture(m_CubeMapPaths[i]);
        m_Current = i;
        glCheckError();
        return 1;
    }
    return 0;
}




