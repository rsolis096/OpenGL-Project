#include "Primitive.h"
#define PI 3.141592653589793238462643383279502884197

Primitive::Primitive() : Object(), m_useEBO(false), m_ebo(0), m_vao(0), m_vbo(0)
{
    //See Object default constructor
    m_Type = "Undefined Primitive";
}

//Used for creating a Primtive with texture information
Primitive::Primitive(const char* type, const char* texturePathDiffuse, const char* texturePathSpecular) : Object(), m_useEBO(false), m_ebo(0), m_vao(0), m_vbo(0)
{
    //Set some rendering properties
    m_hasTexture = true;
    m_Type = type;
    //Open and load diffuse map and specular map, save their Primitives
    m_DiffuseMap = new Texture(texturePathDiffuse, false, "texture_diffuse");
    m_SpecularMap = new Texture(texturePathSpecular, false, "texture_specular");

    //Build the specified Primitive type
    if (type == "Cube")
    {
        m_useEBO = false;
        buildCube();
    }
    else if (type == "Sphere")
    {
        m_useEBO = true;
        buildSphere();
    }
    else
    {
        //Build a cube if invalid primitive is given
        m_useEBO = false;
        buildCube();
    }
}

//Used for creating a primitive with no texture
Primitive::Primitive(const char* type) : Object(), m_useEBO(false), m_ebo(0), m_vao(0), m_vbo(0)
{
    m_hasTexture = false;
    m_Type = type;
    if (type == "Cube")
    {
        m_useEBO = false;
        buildCube();
    }
    else if (type == "Sphere")
    {
        m_useEBO = true;
        buildSphere();
    }
}

void Primitive::Draw(Shader& shader)
{
    shader.use();
    shader.setVec3("object.ambient", m_Ambient);
    shader.setVec3("object.diffuse", m_Diffuse);
    shader.setVec3("object.specular", m_Specular);
    shader.setMat4("model", m_Model);

    //Bind texture and send texture to fragment shader
    if (m_hasTexture)
    {
        glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture (2 texture in frag shader)
        glBindTexture(GL_TEXTURE_2D, m_DiffuseMap->ID);
        glActiveTexture(GL_TEXTURE1); // activate the texture unit first before binding texture (2 texture in frag shader)
        glBindTexture(GL_TEXTURE_2D, m_SpecularMap->ID);

        shader.use();
        shader.setBool("hasTexture", true);
    }
    else
    {
        shader.use();
        shader.setBool("hasTexture", false);
    }

    //Bind Primitive
    glBindVertexArray(m_vao);

    //Choose render type - vertices list (VAO) or indices list(EBO)
    if (m_useEBO) //Sphere
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, 0);
    }
    else //Cube
    {
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    // Unbind buffers and reset state
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

}

void Primitive::buildInterleavedVertices()
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

void Primitive::buildInterleavedVerticesWithTexCoords()
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

void Primitive::buildSphere()
{
    int sectorCount = 30;
    int stackCount = 30;
    int radius = 1.0f;

    //Algorithm provided by (http://www.songho.ca/opengl/gl_sphere.html)
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

void Primitive::buildCube()
{
    //Pre Defined cube vertices, normals, and TexCoords
    m_Vertices = {
        // Back face
        -0.5f, -0.5f, -0.5f, // Bottom-left
         0.5f,  0.5f, -0.5f, // top-right
         0.5f, -0.5f, -0.5f,  // bottom-right         
         0.5f,  0.5f, -0.5f, // top-right
        -0.5f, -0.5f, -0.5f,   // bottom-left
        -0.5f,  0.5f, -0.5f,  // top-left
        // Front face
        -0.5f, -0.5f,  0.5f, // bottom-left
         0.5f, -0.5f,  0.5f,   // bottom-right
         0.5f,  0.5f,  0.5f,  // top-right
         0.5f,  0.5f,  0.5f,   // top-right
        -0.5f,  0.5f,  0.5f,   // top-left
        -0.5f, -0.5f,  0.5f,   // bottom-left
        // Left face
        -0.5f,  0.5f,  0.5f,   // top-right
        -0.5f,  0.5f, -0.5f,   // top-left
        -0.5f, -0.5f, -0.5f, // bottom-left
        -0.5f, -0.5f, -0.5f,   // bottom-left
        -0.5f, -0.5f,  0.5f, // bottom-right
        -0.5f,  0.5f,  0.5f,  // top-right
        // Right face
         0.5f,  0.5f,  0.5f, // top-left
         0.5f, -0.5f, -0.5f,   // bottom-right
         0.5f,  0.5f, -0.5f,   // top-right         
         0.5f, -0.5f, -0.5f,   // bottom-right
         0.5f,  0.5f,  0.5f,   // top-left
         0.5f, -0.5f,  0.5f, // bottom-left     
         // Bottom face
         -0.5f, -0.5f, -0.5f,   // top-right
          0.5f, -0.5f, -0.5f,   // top-left
          0.5f, -0.5f,  0.5f,  // bottom-left
          0.5f, -0.5f,  0.5f,   // bottom-left
         -0.5f, -0.5f,  0.5f, // bottom-right
         -0.5f, -0.5f, -0.5f,   // top-right
         // Top face
         -0.5f,  0.5f, -0.5f,  // top-left
          0.5f,  0.5f,  0.5f,  // bottom-right
          0.5f,  0.5f, -0.5f,   // top-right     
          0.5f,  0.5f,  0.5f,  // bottom-right
         -0.5f,  0.5f, -0.5f,   // top-left
         -0.5f,  0.5f,  0.5f  // bottom-left  
    };

    m_Normals = {
        0.0f,  0.0f, -1.0f,
        0.0f,  0.0f, -1.0f,
        0.0f,  0.0f, -1.0f,
        0.0f,  0.0f, -1.0f,
        0.0f,  0.0f, -1.0f,
        0.0f,  0.0f, -1.0f,

        0.0f,  0.0f,  1.0f,
        0.0f,  0.0f,  1.0f,
        0.0f,  0.0f,  1.0f,
        0.0f,  0.0f,  1.0f,
        0.0f,  0.0f,  1.0f,
        0.0f,  0.0f,  1.0f,

        -1.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,

         1.0f,  0.0f,  0.0f,
         1.0f,  0.0f,  0.0f,
         1.0f,  0.0f,  0.0f,
         1.0f,  0.0f,  0.0f,
         1.0f,  0.0f,  0.0f,
         1.0f,  0.0f,  0.0f,

         0.0f, -1.0f,  0.0f,
         0.0f, -1.0f,  0.0f,
         0.0f, -1.0f,  0.0f,
         0.0f, -1.0f,  0.0f,
         0.0f, -1.0f,  0.0f,
         0.0f, -1.0f,  0.0f,

         0.0f,  1.0f,  0.0f,
         0.0f,  1.0f,  0.0f,
         0.0f,  1.0f,  0.0f,
         0.0f,  1.0f,  0.0f,
         0.0f,  1.0f,  0.0f,
         0.0f,  1.0f,  0.0f
    };

    m_TexCoords = {
        // Back face
        0.0f, 0.0f, // Bottom-left
        1.0f, 1.0f, // top-right
        1.0f, 0.0f, // bottom-right         
        1.0f, 1.0f, // top-right
        0.0f, 0.0f, // bottom-left
        0.0f, 1.0f, // top-left
        // Front face
        0.0f, 0.0f, // bottom-left
        1.0f, 0.0f, // bottom-right
        1.0f, 1.0f, // top-right
        1.0f, 1.0f, // top-right
        0.0f, 1.0f, // top-left
        0.0f, 0.0f, // bottom-left
        // Left face
        1.0f, 0.0f, // top-right
        1.0f, 1.0f, // top-left
        0.0f, 1.0f, // bottom-left
        0.0f, 1.0f, // bottom-left
        0.0f, 0.0f, // bottom-right
        1.0f, 0.0f, // top-right
        // Right face
        1.0f, 0.0f, // top-left
        0.0f, 1.0f, // bottom-right
        1.0f, 1.0f, // top-right         
        0.0f, 1.0f, // bottom-right
        1.0f, 0.0f, // top-left
        0.0f, 0.0f, // bottom-left     
         // Bottom face
        0.0f, 1.0f, // top-right
        1.0f, 1.0f, // top-left
        1.0f, 0.0f, // bottom-left
        1.0f, 0.0f, // bottom-left
        0.0f, 0.0f, // bottom-right
        0.0f, 1.0f, // top-right
         // Top face
        0.0f, 1.0f, // top-left
        1.0f, 0.0f, // bottom-right
        1.0f, 1.0f, // top-right     
        1.0f, 0.0f, // bottom-right
        0.0f, 1.0f, // top-left
        0.0f, 0.0f  // bottom-left   
    };

    //Combine above mesh data
    if (m_hasTexture == true)
        buildInterleavedVerticesWithTexCoords();
    else
        buildInterleavedVertices();

    //Generate VAO and VBO
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    //Assign vertices to Primitive
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_InterleavedVertices.size(), m_InterleavedVertices.data(), GL_STATIC_DRAW);

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
}

