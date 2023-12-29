#include "SkyBox.h"

SkyBox::SkyBox(Shader& s, Camera* c) : m_CubeMapShader(s), m_PlayerCamera(c)
{
    //Skybox faces
    m_Faces = {
        "Assets/skybox/right.jpg",
        "Assets/skybox/left.jpg",
        "Assets/skybox/top.jpg",
        "Assets/skybox/bottom.jpg",
        "Assets/skybox/front.jpg",
        "Assets/skybox/back.jpg"
    };

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
    m_CubeMap = new Texture(m_Faces);
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
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_CubeMap->ID);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS); // set depth function back to default
    glCheckError();
}

int SkyBox::setCubeMapTexture(std::vector<std::string> v)
{
    if (v.size() != 6)
    {
        delete m_CubeMap;
        m_CubeMap = nullptr;
        m_CubeMap = new Texture(v);
    }

    /*
    m_Vertices = {
        // positions
        // Face: Positive X
        -10.0f,  10.0f, -10.0f,
        -10.0f, -10.0f, -10.0f,
        -10.0f, -10.0f,  10.0f,
        -10.0f,  10.0f,  10.0f,

        // Face: Negative X
         10.0f,  10.0f,  10.0f,
         10.0f, -10.0f,  10.0f,
         10.0f, -10.0f, -10.0f,
         10.0f,  10.0f, -10.0f,

         // Face: Positive Y
         -10.0f,  10.0f, -10.0f,
         -10.0f,  10.0f,  10.0f,
          10.0f,  10.0f,  10.0f,
          10.0f,  10.0f, -10.0f,

          // Face: Negative Y
          -10.0f, -10.0f, -10.0f,
           10.0f, -10.0f, -10.0f,
           10.0f, -10.0f,  10.0f,
          -10.0f, -10.0f,  10.0f,

          // Face: Positive Z
          -10.0f,  10.0f,  10.0f,
          -10.0f, -10.0f,  10.0f,
           10.0f, -10.0f,  10.0f,
           10.0f,  10.0f,  10.0f,

           // Face: Negative Z
            10.0f,  10.0f, -10.0f,
            10.0f, -10.0f, -10.0f,
           -10.0f, -10.0f, -10.0f,
           -10.0f,  10.0f, -10.0f
    };
    */

    //glBindBuffer(GL_ARRAY_BUFFER, m_SkyBoxVBO);
    //glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(float), &m_Vertices[0], GL_STATIC_DRAW);
    //glEnableVertexAttribArray(0);
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
       
    return 0;
}

int SkyBox::setCubeMapTexture(std::string s)
{
    if (s.size() != 0)
    {
        delete m_CubeMap;
        m_CubeMap = nullptr;
        m_CubeMap = new Texture(s);
    }
    return 0;
}


