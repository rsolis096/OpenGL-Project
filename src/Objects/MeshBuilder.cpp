#include "Objects/MeshBuilder.h"

#include <cmath>
#include <cstddef>

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

MeshData MeshBuilder::build(PrimitiveKind kind)
{
    switch (kind)
    {
        case PrimitiveKind::Cube:
            return buildCubeMesh();

        case PrimitiveKind::Sphere:
            return buildSphereMesh();

        case PrimitiveKind::Plane:
            return buildPlaneMesh();
    }

    return MeshData();
}

MeshData MeshBuilder::buildCubeMesh()
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

MeshData MeshBuilder::buildSphereMesh()
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

MeshData MeshBuilder::buildPlaneMesh()
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
