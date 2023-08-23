#include "Object.h"

Object::Object(unsigned int sId, const char* texturePathDiffuse, const char* texturePathSpecular)
{
    m_Ambient = glm::vec3(1.0f);
    m_Diffuse = glm::vec3(1.0f);
    m_Specular = glm::vec3(1.0f);
    m_Position = glm::vec3(0.0f, 0.0f, 0.0f);

    m_shaderID = sId;
    m_hasTexture = true;
    m_useEBO = false;

    //Open and load diffuse map
    if (std::strstr(texturePathDiffuse, ".jpg"))
        diffuseMap = new Texture(texturePathDiffuse, false, GL_RGB);
    else
        diffuseMap = new Texture(texturePathDiffuse, false, GL_RGBA);

    //Open and load specular map
    if (std::strstr(texturePathSpecular, ".jpg"))
        specularMap = new Texture(texturePathSpecular, false, GL_RGB);
    else
        specularMap = new Texture(texturePathSpecular, false, GL_RGBA);

    //Generate VAO and VBO
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
}

Object::Object(unsigned int sId)
{
    m_Ambient = glm::vec3(1.0f);
    m_Diffuse = glm::vec3(1.0f);
    m_Specular = glm::vec3(1.0f);
    m_Position = glm::vec3(0.0f, 0.0f, 0.0f);

    m_shaderID = sId;
    m_hasTexture = false;
    m_useEBO = false;

}

void Object::buildInterleavedVertices()
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

void Object::buildInterleavedVerticesWithTexCoords()
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


void Object::render()
{

    glUseProgram(m_shaderID);
    glUniform3fv(glGetUniformLocation(m_shaderID, "object.ambient"), 1, &m_Ambient[0]);
    glUniform3fv(glGetUniformLocation(m_shaderID, "object.diffuse"), 1, &m_Diffuse[0]);
    glUniform3fv(glGetUniformLocation(m_shaderID, "object.specular"), 1, &m_Specular[0]);

    //Bind texture and send texture to fragment shader
    if (m_hasTexture)
    {
        glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture (2 texture in frag shader)
        glBindTexture(GL_TEXTURE_2D, diffuseMap->ID);
        glActiveTexture(GL_TEXTURE1); // activate the texture unit first before binding texture (2 texture in frag shader)
        glBindTexture(GL_TEXTURE_2D, specularMap->ID);

        glUseProgram(m_shaderID);
        glUniform1i(glGetUniformLocation(m_shaderID, "hasTexture"), true);
    }
    else
    {
        glUseProgram(m_shaderID);
        glUniform1i(glGetUniformLocation(m_shaderID, "hasTexture"), false);
    }

    //Bind object
    glBindVertexArray(m_vao);

    //Choose render type - vertices list (VAO) or indices list(EBO)
    if (m_useEBO)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        //Render the object
        glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, 0);
    }
    else
    {
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    // Unbind buffers and reset state
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

}
