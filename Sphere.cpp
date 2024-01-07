#include "Sphere.h"
#define PI 3.141592653589793238462643383279502884197
unsigned int Sphere::sphereCount = 0;

//Used for creating a Primtive with texture information
Sphere::Sphere(const char* texturePathDiffuse, const char* texturePathSpecular) : Object()
{
    //Set some rendering properties
    m_hasTexture = true;
    m_DisplayName = "Sphere" + std::to_string(sphereCount);
    m_ObjectID = sphereCount;
    //Open and load diffuse map and specular map, save their Spheres
    m_DiffuseMap = new Texture(texturePathDiffuse, false, "texture_diffuse");
    m_SpecularMap = new Texture(texturePathSpecular, false, "texture_specular");

    //Build the specified Sphere type
    buildSphere();
    sphereCount++;
}

//Used for creating a primitive with no texture
Sphere::Sphere() : Object()
{
    m_hasTexture = false;
    m_DisplayName = "Sphere" + std::to_string(sphereCount);
    m_ObjectID = sphereCount;
    sphereCount++;
    buildSphere();
}


void Sphere::Draw(Shader& shader)
{
    shader.use();
    shader.setVec3("object.ambient", m_Ambient);
    shader.setVec3("object.diffuse", m_Diffuse);
    shader.setVec3("object.specular", m_Specular);
    shader.setMat4("model", m_Model);
    shader.setBool("hasTexture", m_hasTexture);

    //Bind texture and send texture to fragment shader
    if (m_hasTexture)
    {
        glActiveTexture(GL_TEXTURE3); // activate the texture unit first before binding texture (2 texture in frag shader)
        glBindTexture(GL_TEXTURE_2D, m_DiffuseMap->ID);
        glActiveTexture(GL_TEXTURE4); // activate the texture unit first before binding texture (2 texture in frag shader)
        glBindTexture(GL_TEXTURE_2D, m_SpecularMap->ID);

        GLint diffuseLocation = glGetUniformLocation(shader.ID, "material.diffuse");
        GLint specularLocation = glGetUniformLocation(shader.ID, "material.specular");

        if (diffuseLocation != -1)
            glUniform1i(diffuseLocation, 3); // 0 corresponds to GL_TEXTURE0

        if (specularLocation != -1)
            glUniform1i(specularLocation, 4); // 1 corresponds to GL_TEXTURE1
    }


    //Bind Sphere
    glBindVertexArray(m_vao);

    //Choose render type - indices list(EBO)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, 0);

    // Unbind buffers and reset state
    glBindVertexArray(0);
    glCheckError();

}

void Sphere::ShadowMapDraw(Shader& shader)
{
    shader.use();

    shader.setMat4("model", m_Model);
    shader.setBool("hasTexture", false);

    //Bind Sphere
    glBindVertexArray(m_vao);

    //Choose render type - indices list(EBO)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, 0);

    // Unbind buffers and reset state
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glCheckError();

}


void Sphere::buildSphere()
{
    int sectorCount = 35;
    int stackCount = 35;
    int radius = 1.0f;

    //Algorithm provided by (http://www.songho.ca/opengl/gl_sphere.html)
    std::vector<float>().swap(m_Vertices);
    std::vector<float>().swap(m_Normals);
    std::vector<float>().swap(m_TexCoords);

    float x, y, z;                              // vertex position
    float s, t;                                 // vertex texCoord
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
            nx = x  * lengthInv;
            ny = y  * lengthInv;
            nz = z  * lengthInv;
            m_Normals.push_back(nx);
            m_Normals.push_back(ny);
            m_Normals.push_back(nz);


            // vertex tex coord (s, t) range between [0, 1]
            s = (float)j / sectorCount;
            t = (float)i / stackCount;
            m_TexCoords.push_back(s);
            m_TexCoords.push_back(t);


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
    buildInterleavedVerticesWithTexCoords();


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
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

int Sphere::updateTexture(std::vector<std::string> texturePaths)
{
    // Three Scenarios
    // 1. Updating a texture of an Object that already has textures
    // 2. Updating a texture of an Object with no initial texture
    // 3. Updating the texture of a model object

    //Secenario 1
    if (m_hasTexture == true)
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
            //Texture failed to apply
            //As of right now, all textures are deleted
            delete m_DiffuseMap;
            delete m_SpecularMap;
            m_DiffuseMap = nullptr;
            m_SpecularMap = nullptr;
            m_hasTexture = false;
            return 0;
        }
        else
            std::cout << "Updated Texture successfully!" << std::endl;
        return 1;
    }
    //Scenario 2
    else if (m_hasTexture == false)
    {
        m_DiffuseMap = new Texture(texturePaths[0].c_str(), false, "texture_diffuse");
        m_SpecularMap = new Texture(texturePaths[1].c_str(), false, "texture_specular");
        if (m_DiffuseMap->ID == GL_INVALID_VALUE || m_SpecularMap->ID == GL_INVALID_VALUE)
        {
            //Texture failed to apply
            delete m_DiffuseMap;
            delete m_SpecularMap;
            m_DiffuseMap = nullptr;
            m_SpecularMap = nullptr;
            m_hasTexture = false;
            return 0;
        }
        else
            m_hasTexture = true;
        return 1;
    }
    return 0;
}


