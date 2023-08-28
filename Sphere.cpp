
#include "Sphere.h"
#define PI 3.141592653589793238462643383279502884197


Sphere::Sphere(Shader& shader)
{
    //Set Default Object Properties
    m_Ambient = glm::vec3(0.2f);
    m_Diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
    m_Specular = glm::vec3(0.5f);
    m_Position = glm::vec3(1.0f, 1.0f, 1.0f);
    model = glm::mat4(1.0f);
    model = glm::translate(model, m_Position);

    //Set some rendering properties
    //m_Shader = shader;
    m_hasTexture = false;
    m_type = "Sphere";

    buildSphere();
}

Sphere::Sphere(const char* texturePathDiffuse, const char* texturePathSpecular, Shader& shader)
{
    //Set default object properties
    m_Ambient = glm::vec3(0.2f);
    m_Diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
    m_Specular = glm::vec3(0.5f);
    m_Position = glm::vec3(1.0f, 1.0f, 1.0f);
    model = glm::mat4(1.0f);
    model = glm::translate(model, m_Position);

    //Set some rendering properties
    //m_Shader = shader;
    m_hasTexture = true;
    m_type = "Sphere";

    //Open and load diffuse map and specular map, save their objects
    if (std::strstr(texturePathDiffuse, ".jpg"))
        diffuseMap = new Texture(texturePathDiffuse, false, GL_RGB);
    else
        diffuseMap = new Texture(texturePathDiffuse, false, GL_RGBA);

    //Open and load specular map
    if (std::strstr(texturePathSpecular, ".jpg"))
        specularMap = new Texture(texturePathSpecular, false, GL_RGB);
    else
        specularMap = new Texture(texturePathSpecular, false, GL_RGBA);

    buildSphere();
}

void Sphere::Draw(Shader& shader)
{
    shader.use();
    shader.setVec3("object.ambient", m_Ambient);
    shader.setVec3("object.diffuse", m_Diffuse);
    shader.setVec3("object.specular", m_Specular);
    shader.setMat4("model", model);

    //Bind texture and send texture to fragment shader
    if (m_hasTexture)
    {
        glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture (2 texture in frag shader)
        glBindTexture(GL_TEXTURE_2D, diffuseMap->ID);
        glActiveTexture(GL_TEXTURE1); // activate the texture unit first before binding texture (2 texture in frag shader)
        glBindTexture(GL_TEXTURE_2D, specularMap->ID);

        shader.use();
        shader.setBool("hasTexture", true);
    }
    else
    {
        shader.use();
        shader.setBool("hasTexture", false);
    }

    //Bind object
    glBindVertexArray(m_vao);

    //Choose render type - vertices list (VAO) or indices list(EBO)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    //Render the object
    glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, 0);

    // Unbind buffers and reset state
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Sphere::buildSphere()
{
    int sectorCount = 30;
    int stackCount = 30;
    int radius = 1.0f;

    //Algorithm provided by site given in assignment (http://www.songho.ca/opengl/gl_sphere.html)
    std::vector<float>().swap(m_Vertices);
    std::vector<float>().swap(m_Normals);
    std::vector<float>().swap(m_TexCoords);

    float x, y, z, xy;                              // vertex position
    float s, t;                                     // vertex texCoord
    float nx, ny, nz;

    float sectorStep = 2 * PI / sectorCount;
    float stackStep = PI / stackCount;
    float sectorAngle, stackAngle;
    float lengthInv = 1.0f / radius;

    for (int i = 0; i <= stackCount; ++i)
    {
        stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
        z = radius * sinf(stackAngle);              // r * sin(u)
        // add (sectorCount+1) vertices per stack
        // first and last vertices have same position and normal, but different tex coords
        for (int j = 0; j <= sectorCount; ++j)
        {
            sectorAngle = j * sectorStep;          // starting from 0 to 2pi

            // vertex position (x, y, z)
            x = radius * cosf(stackAngle) * cosf(sectorAngle);
            y = radius * cosf(stackAngle) * sinf(sectorAngle);
            m_Vertices.push_back(x);
            m_Vertices.push_back(y);
            m_Vertices.push_back(z);

            // normalized vertex normal (nx, ny, nz)
            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;
            m_Normals.push_back(nx);
            m_Normals.push_back(ny);
            m_Normals.push_back(nz);

            if (m_hasTexture)
            {
                // vertex tex coord (s, t) range between [0, 1]
                s = (float)j / sectorCount;
                t = (float)i / stackCount;
                m_TexCoords.push_back(s);
                m_TexCoords.push_back(t);
            }

        }
    }

    int k1, k2;
    for (int i = 0; i < stackCount; ++i)
    {
        k1 = i * (stackCount + 1);     // beginning of current stack
        k2 = k1 + sectorCount + 1;      // beginning of next stack

        for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
        {
            // 2 triangles per sector excluding first and last stacks
            // k1 => k2 => k1+1
            if (i != 0)
            {
                m_Indices.push_back(k1);
                m_Indices.push_back(k2);
                m_Indices.push_back(k1 + 1);
            }

            // k1+1 => k2 => k2+1
            if (i != (stackCount - 1))
            {
                m_Indices.push_back(k1 + 1);
                m_Indices.push_back(k2);
                m_Indices.push_back(k2 + 1);
            }
        }
    }

    // generate interleaved vertex array as well
    if (m_hasTexture)
        buildInterleavedVerticesWithTexCoords();
    else
        buildInterleavedVertices();

    //Setup VAO and VBO
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    //Bind VAO
    glBindVertexArray(m_vao);

    //Bind and fill VBOs with data
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_InterleavedVertices.size() * sizeof(float), m_InterleavedVertices.data(), GL_STATIC_DRAW);

    // Bind and fill EBO with indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * m_Indices.size(), m_Indices.data(), GL_STATIC_DRAW);

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
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

