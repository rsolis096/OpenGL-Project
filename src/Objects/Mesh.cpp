#include "Objects/Mesh.h"

#include "DebugUtils.h"
#include "Lighting/Shader.h"

Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<ModelTexture> textures)
{
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;

    setupMesh();
}

// Move constructor
Mesh::Mesh(Mesh&& other)
    : vertices(std::move(other.vertices)),
    indices(std::move(other.indices)),
    textures(std::move(other.textures)),
    VAO(other.VAO),
    VBO(other.VBO),
    EBO(other.EBO)
{
    other.VAO = 0;
    other.VBO = 0;
    other.EBO = 0;
}


// Move assignment operator
Mesh& Mesh::operator=(Mesh&& other)
{
    std::cout << "Move assignment operator called\n";
    if (this != &other)
    {
        releaseGpuResources();

        vertices = std::move(other.vertices);
        indices = std::move(other.indices);
        textures = std::move(other.textures);
        VAO = other.VAO;
        VBO = other.VBO;
        EBO = other.EBO;

        other.VAO = 0;
        other.VBO = 0;
        other.EBO = 0;
    }
    return *this;
}

Mesh::~Mesh()
{
    //std::cout << "Mesh Destructor called\n";
    releaseGpuResources();

    vertices.clear();
    vertices.shrink_to_fit();
    indices.clear();
    indices.shrink_to_fit();
    textures.clear();
    textures.shrink_to_fit();
}

void Mesh::releaseGpuResources()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    VAO = 0;
    VBO = 0;
    EBO = 0;
}

void Mesh::ShadowPassDraw(Shader& shader) const
{
    DrawMesh();
}

void Mesh::DrawGeometryPass(Shader& shader, bool hasTexture) const
{
    ApplyMaterialUniforms(shader, hasTexture);
    DrawMesh();
}

// render the mesh
void Mesh::Draw(Shader& shader, bool hasTexture) const
{
    ApplyMaterialUniforms(shader, hasTexture);
    DrawMesh();
}

void Mesh::ApplyMaterialUniforms(Shader& shader, bool hasTexture) const
{
    shader.use();
    shader.setBool("hasTexture", hasTexture);

    // bind appropriate textures
    unsigned int normalNr = 1;
    unsigned int heightNr = 1;

    bool hasBindedDiffuse = false;
    bool hasBindedSpecular = false;
    bool hasBindedNormal = false;

    if (hasTexture)
    {
        for (unsigned int i = 0; i < textures.size(); i++)
        {
            std::string number;
            std::string name = textures[i].type;
            GLint location;
            if (name == "texture_diffuse") {
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, textures[i].id);
                location = glGetUniformLocation(shader.m_ProgramId, "material.diffuse");
                glUniform1i(location, 1);
            }
            else if (name == "texture_specular") {
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, textures[i].id);
                location = glGetUniformLocation(shader.m_ProgramId, "material.specular");
                glUniform1i(location, 2);
            }
            else if (name == "texture_normal") {
                glActiveTexture(GL_TEXTURE3);
                glBindTexture(GL_TEXTURE_2D, textures[i].id);
                number = std::to_string(normalNr++); // Convert unsigned int to string
                location = glGetUniformLocation(shader.m_ProgramId, ("material." + name + number).c_str());
                glUniform1i(location, 3);
            }
            /*
            else if (name == "texture_height") {
                glActiveTexture(GL_TEXTURE10);
                glBindTexture(GL_TEXTURE_2D, textures[i].id);
                number = std::to_string(heightNr++); // Convert unsigned int to string
                location = glGetUniformLocation(shader.m_ProgramId, ("material." + name + number).c_str());
                glUniform1i(location, 10);
            }
            else {
                std::cout << "continued \n";
                continue;
            }
            */
        }
    }
}

void Mesh::DrawMesh() const
{
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
    glCheckError();
}

// From LearnOpenGL
void Mesh::setupMesh()
{
    // create buffers/arrays
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    // load data into vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // set the vertex attribute pointers
    // vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    // vertex tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
    // vertex bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
    // ids
    glEnableVertexAttribArray(5);
    glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));
    // weights
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));
    glBindVertexArray(0);
}




namespace
{
    constexpr float Pi = 3.14159265358979323846f;

    Vertex makeVertex(
        const glm::vec3& position,
        const glm::vec3& normal,
        const glm::vec2& texCoords)
    {
        Vertex vertex{};
        vertex.Position = position;
        vertex.Normal = normal;
        vertex.TexCoords = texCoords;
        return vertex;
    }
}

MeshData buildCubeMesh()
{
    const std::vector<float> positions = {
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

    const std::vector<float> normals = {
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

    const std::vector<float> texCoords = {
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

    MeshData meshData;
    const std::size_t vertexCount = positions.size() / 3;
    meshData.vertices.reserve(vertexCount);
    meshData.indices.reserve(vertexCount);

    for (std::size_t i = 0; i < vertexCount; ++i)
    {
        const std::size_t positionOffset = i * 3;
        const std::size_t texCoordOffset = i * 2;

        meshData.vertices.push_back(makeVertex(
            { positions[positionOffset], positions[positionOffset + 1], positions[positionOffset + 2] },
            { normals[positionOffset], normals[positionOffset + 1], normals[positionOffset + 2] },
            { texCoords[texCoordOffset], texCoords[texCoordOffset + 1] }));

        meshData.indices.push_back(static_cast<unsigned int>(i));
    }

    return meshData;
}

MeshData buildSphereMesh()
{
    const int sectorCount = 35;
    const int stackCount = 35;
    const float radius = 1.0f;

    //Algorithm provided by (http://www.songho.ca/opengl/gl_sphere.html)

    float x, y, z;                              // vertex position
    float s, t;                                 // vertex texCoord
    float nx, ny, nz;

    const float sectorStep = 2.0f * Pi / static_cast<float>(sectorCount);
    const float stackStep = Pi / static_cast<float>(stackCount);
    float sectorAngle, stackAngle;
    const float lengthInv = 1.0f / radius;

    MeshData meshData;

    for (int i = 0; i <= stackCount; ++i)
    {
        stackAngle = Pi / 2.0f - static_cast<float>(i) * stackStep; // starting from pi/2 to -pi/2
        z = radius * sinf(stackAngle);              // r * sin(u)
        // add (sectorCount+1) vertices per stack
        // first and last vertices have same position and normal, but different tex coords
        for (int j = 0; j <= sectorCount; ++j)
        {
            sectorAngle = static_cast<float>(j) * sectorStep;      // starting from 0 to 2pi

            // vertex position (x, y, z)
            x = radius * cosf(stackAngle) * cosf(sectorAngle);
            y = radius * cosf(stackAngle) * sinf(sectorAngle);

            // normalized vertex normal (nx, ny, nz)
            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;

            // vertex tex coord (s, t) range between [0, 1]
            s = static_cast<float>(j) / static_cast<float>(sectorCount);
            t = static_cast<float>(i) / static_cast<float>(stackCount);

            meshData.vertices.push_back(makeVertex(
                { x, y, z },
                { nx, ny, nz },
                { s, t }));
        }
    }

    int k1, k2;
    for (int i = 0; i < stackCount; ++i)
    {
        k1 = i * (sectorCount + 1);      // beginning of current stack
        k2 = k1 + sectorCount + 1;       // beginning of next stack

        for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
        {
            // 2 triangles per sector excluding first and last stacks
            // k1 => k2 => k1+1
            if (i != 0)
            {
                meshData.indices.push_back(static_cast<unsigned int>(k1));
                meshData.indices.push_back(static_cast<unsigned int>(k2));
                meshData.indices.push_back(static_cast<unsigned int>(k1 + 1));
            }

            // k1+1 => k2 => k2+1
            if (i != (stackCount - 1))
            {

                meshData.indices.push_back(static_cast<unsigned int>(k1 + 1));
                meshData.indices.push_back(static_cast<unsigned int>(k2));
                meshData.indices.push_back(static_cast<unsigned int>(k2 + 1));
            }
        }
    }

    return meshData;
}

MeshData buildPlaneMesh()
{
    // Predefined plane positions, normals, and texture coordinates.
    const std::vector<float> positions = {
         25.0f, -0.5f,  25.0f,
        -25.0f, -0.5f,  25.0f,
        -25.0f, -0.5f, -25.0f,
         25.0f, -0.5f,  25.0f,
        -25.0f, -0.5f, -25.0f,
         25.0f, -0.5f, -25.0f
    };

    const std::vector<float> normals = {
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
    };

    const std::vector<float> texCoords = {
        25.0f,  0.0f,
        0.0f,  0.0f,
        0.0f, 25.0f,
        25.0f,  0.0f,
        0.0f, 25.0f,
        25.0f, 25.0f
    };

    MeshData meshData;
    const std::size_t vertexCount = positions.size() / 3;
    meshData.vertices.reserve(vertexCount);
    meshData.indices.reserve(vertexCount);

    for (std::size_t i = 0; i < vertexCount; ++i)
    {
        const std::size_t positionOffset = i * 3;
        const std::size_t texCoordOffset = i * 2;

        meshData.vertices.push_back(makeVertex(
            { positions[positionOffset], positions[positionOffset + 1], positions[positionOffset + 2] },
            { normals[positionOffset], normals[positionOffset + 1], normals[positionOffset + 2] },
            { texCoords[texCoordOffset], texCoords[texCoordOffset + 1] }));

        meshData.indices.push_back(static_cast<unsigned int>(i));
    }

    return meshData;
}
