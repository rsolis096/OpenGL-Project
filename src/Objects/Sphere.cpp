#include "Objects/Sphere.h"

#include "DebugUtils.h"
#include "Lighting/Shader.h"
#include "Objects/Texture.h"
#define PI 3.141592653589793238462643383279502884197


//Used for creating a Primitive with texture information
Sphere::Sphere(const char* texturePathDiffuse, const char* texturePathSpecular) : Object()
{
    // Initialize the unique_ptrs using std::make_unique
    m_Material.setTextures({ texturePathDiffuse , texturePathSpecular });

    // Build the specified Sphere type
    buildSphere();
}

//Used for creating a primitive with no texture
Sphere::Sphere() : Object()
{
    buildSphere();
}

void Sphere::Draw(Shader& shader)
{
    shader.use();
    shader.setMat4("model", m_Transform.matrix());

    ApplyMaterialUniforms(shader);
    DrawMesh();
}

void Sphere::ShadowPassDraw(Shader& shader)
{
    shader.use();
    shader.setMat4("model", m_Transform.matrix());

    DrawMesh();
}

void Sphere::DrawGeometryPass(Shader& shader)
{
    shader.use();
    shader.setMat4("model", m_Transform.matrix());

    ApplyMaterialUniforms(shader);
    DrawMesh();
}

void Sphere::ApplyMaterialUniforms(Shader& shader)
{
    shader.use();
    shader.setMaterial(m_Material);

    if (m_Material.hasTextures())
    {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_Material.m_DiffuseMap->ID);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, m_Material.m_SpecularMap->ID);
    }
}

void Sphere::DrawMesh()
{
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, nullptr);

    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

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