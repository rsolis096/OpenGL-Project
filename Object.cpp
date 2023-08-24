#include "Object.h"

Object::Object()
{
    //Set default object properties
    m_Ambient = glm::vec3(0.2f);
    m_Diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
    m_Specular = glm::vec3(0.5f);
    m_Position = glm::vec3(1.0f, 1.0f, 1.0f);
}

void Object::render()
{

    m_shader.use();
    m_shader.setVec3("object.ambient", m_Ambient);
    m_shader.setVec3("object.diffuse", m_Diffuse);
    m_shader.setVec3("object.specular", m_Specular);
    m_shader.setMat4("model", model);

    //Bind texture and send texture to fragment shader
    if (m_hasTexture)
    {
        glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture (2 texture in frag shader)
        glBindTexture(GL_TEXTURE_2D, diffuseMap->ID);
        glActiveTexture(GL_TEXTURE1); // activate the texture unit first before binding texture (2 texture in frag shader)
        glBindTexture(GL_TEXTURE_2D, specularMap->ID);

        m_shader.use();
        m_shader.setBool("hasTexture", true);
    }
    else
    {
        m_shader.use();
        m_shader.setBool("hasTexture", false);
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

void Object::setAmbient(glm::vec3 newAmbient)
{
    m_shader.use();
    m_Ambient = newAmbient;
    m_shader.setVec3("object.ambient", m_Ambient);
}

void Object::setDiffuse(glm::vec3 newDiffuse)
{
    m_shader.use();
    m_Diffuse = newDiffuse;
    m_shader.setVec3("object.diffuse", m_Diffuse);
}

void Object::setSpecular(glm::vec3 newSpecular)
{
    m_shader.use();
    m_Specular = newSpecular;
    m_shader.setVec3("object.specular", m_Specular);
}

void Object::setPosition(glm::vec3 newPosition)
{
    model = glm::mat4(1.0f);
    model = glm::translate(model, newPosition);
    m_shader.use();
    m_shader.setMat4("model", model);
    //glUniformMatrix4fv(glGetUniformLocation(m_shaderID, "model"), 1, GL_FALSE, &model[0][0]);
}

void Object::translatePosition(glm::vec3 newPosition)
{
    m_Position[0] += newPosition[0];
    m_Position[1] += newPosition[1];
    m_Position[2] += newPosition[2];

    model = glm::mat4(1.0f);
    model = glm::translate(model, m_Position);
}
