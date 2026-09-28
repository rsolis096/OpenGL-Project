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

    std::vector<glm::vec2> makeUnitCircle(int sectorCount)
    {
        std::vector<glm::vec2> unitCircle;
        unitCircle.reserve(static_cast<std::size_t>(sectorCount) + 1);

        const float sectorStep = 2.0f * Pi / static_cast<float>(sectorCount);
        for (int i = 0; i <= sectorCount; ++i)
        {
            const float angle = static_cast<float>(i) * sectorStep;
            unitCircle.emplace_back(std::cos(angle), std::sin(angle));
        }

        return unitCircle;
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

        case PrimitiveKind::Cylinder:
            return buildCylinderMesh();

        case PrimitiveKind::Cone:
            return buildConeMesh();

        case PrimitiveKind::Torus:
            return buildTorusMesh();
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

MeshData MeshBuilder::buildCylinderMesh()
{

    // https://www.songho.ca/opengl/gl_cylinder.html

    constexpr int sectorCount = 64;
    constexpr float radius = 0.5f;
    constexpr float height = 1.0f;
    constexpr float halfHeight = height * 0.5f;

    MeshData meshData;
    const std::vector<glm::vec2> unitCircle = makeUnitCircle(sectorCount);

    // Side vertices: one complete ring for the base and one for the top.
    // The repeated final vertex closes the UV seam.
    for (int i = 0; i < 2; ++i)
    {
        const float y = -halfHeight + static_cast<float>(i) * height;
        const float t = 1.0f - static_cast<float>(i);

        for (int j = 0; j <= sectorCount; ++j)
        {
            const glm::vec2 unit = unitCircle[j];
            meshData.vertices.push_back(makeVertex(
                { unit.x * radius, y, unit.y * radius },
                { unit.x, 0.0f, unit.y },
                { static_cast<float>(j) / static_cast<float>(sectorCount), t }));
        }
    }

    unsigned int base = 0;
    unsigned int top = static_cast<unsigned int>(sectorCount + 1);
    for (int i = 0; i < sectorCount; ++i, ++base, ++top)
    {
        meshData.indices.insert(meshData.indices.end(), {
            base, top, base + 1,
            top, top + 1, base + 1
        });
    }

    // Cap vertices are separate from the side so their normals remain flat.
    const unsigned int baseCenter = static_cast<unsigned int>(meshData.vertices.size());
    for (int i = 0; i < 2; ++i)
    {
        const float y = -halfHeight + static_cast<float>(i) * height;
        const float normalY = -1.0f + static_cast<float>(i) * 2.0f;

        meshData.vertices.push_back(makeVertex(
            { 0.0f, y, 0.0f }, { 0.0f, normalY, 0.0f }, { 0.5f, 0.5f }));

        for (int j = 0; j < sectorCount; ++j)
        {
            const glm::vec2 unit = unitCircle[j];
            meshData.vertices.push_back(makeVertex(
                { unit.x * radius, y, unit.y * radius },
                { 0.0f, normalY, 0.0f },
                { -unit.x * 0.5f + 0.5f, -unit.y * 0.5f + 0.5f }));
        }
    }

    const unsigned int baseRing = baseCenter + 1;
    for (int i = 0; i < sectorCount; ++i)
    {
        const unsigned int current = baseRing + static_cast<unsigned int>(i);
        const unsigned int next = baseRing + static_cast<unsigned int>((i + 1) % sectorCount);
        meshData.indices.insert(meshData.indices.end(), { baseCenter, current, next });
    }

    const unsigned int topCenter = baseCenter + static_cast<unsigned int>(sectorCount + 1);
    const unsigned int topRing = topCenter + 1;
    for (int i = 0; i < sectorCount; ++i)
    {
        const unsigned int current = topRing + static_cast<unsigned int>(i);
        const unsigned int next = topRing + static_cast<unsigned int>((i + 1) % sectorCount);
        meshData.indices.insert(meshData.indices.end(), { topCenter, next, current });
    }

    return meshData;
}

MeshData MeshBuilder::buildConeMesh()
{

    // https://www.songho.ca/opengl/gl_cone.html

    constexpr int sectorCount = 64;
    constexpr int stackCount = 18;
    constexpr float baseRadius = 0.5f;
    constexpr float height = 1.0f;
    constexpr float halfHeight = height * 0.5f;

    MeshData meshData;
    const std::vector<glm::vec2> unitCircle = makeUnitCircle(sectorCount);

    const float slopeAngle = std::atan2(baseRadius, height);
    const float radialNormal = std::cos(slopeAngle);
    const float verticalNormal = std::sin(slopeAngle);

    // Multiple stacks keep the unavoidable apex normal/UV convergence local
    // to a small area at the tip while preserving smooth side normals.
    for (int i = 0; i <= stackCount; ++i)
    {
        const float stackFraction = static_cast<float>(i) / static_cast<float>(stackCount);
        const float y = -halfHeight + stackFraction * height;
        const float radius = baseRadius * (1.0f - stackFraction);
        const float t = 1.0f - stackFraction;

        for (int j = 0; j <= sectorCount; ++j)
        {
            const glm::vec2 unit = unitCircle[j];
            meshData.vertices.push_back(makeVertex(
                { unit.x * radius, y, unit.y * radius },
                { unit.x * radialNormal, verticalNormal, unit.y * radialNormal },
                { static_cast<float>(j) / static_cast<float>(sectorCount), t }));
        }
    }

    for (int i = 0; i < stackCount; ++i)
    {
        unsigned int current = static_cast<unsigned int>(i * (sectorCount + 1));
        unsigned int nextStack = current + static_cast<unsigned int>(sectorCount + 1);

        for (int j = 0; j < sectorCount; ++j, ++current, ++nextStack)
        {
            meshData.indices.insert(meshData.indices.end(), {
                current, nextStack, current + 1
            });

            if (i < stackCount - 1)
            {
                meshData.indices.insert(meshData.indices.end(), {
                    nextStack, nextStack + 1, current + 1
                });
            }
        }
    }

    const unsigned int baseCenter = static_cast<unsigned int>(meshData.vertices.size());
    meshData.vertices.push_back(makeVertex(
        { 0.0f, -halfHeight, 0.0f }, { 0.0f, -1.0f, 0.0f }, { 0.5f, 0.5f }));

    for (int i = 0; i < sectorCount; ++i)
    {
        const glm::vec2 unit = unitCircle[i];
        meshData.vertices.push_back(makeVertex(
            { unit.x * baseRadius, -halfHeight, unit.y * baseRadius },
            { 0.0f, -1.0f, 0.0f },
            { -unit.x * 0.5f + 0.5f, -unit.y * 0.5f + 0.5f }));
    }

    const unsigned int baseRing = baseCenter + 1;
    for (int i = 0; i < sectorCount; ++i)
    {
        const unsigned int current = baseRing + static_cast<unsigned int>(i);
        const unsigned int next = baseRing + static_cast<unsigned int>((i + 1) % sectorCount);
        meshData.indices.insert(meshData.indices.end(), { baseCenter, current, next });
    }

    return meshData;
}

MeshData MeshBuilder::buildTorusMesh()
{
    // https://www.songho.ca/opengl/gl_torus.html

    constexpr int sectorCount = 64;
    constexpr int sideCount = 32;
    constexpr float majorRadius = 0.75f;
    constexpr float minorRadius = 0.25f;

    MeshData meshData;
    const float sectorStep = 2.0f * Pi / static_cast<float>(sectorCount);
    const float sideStep = 2.0f * Pi / static_cast<float>(sideCount);

    for (int i = 0; i <= sideCount; ++i)
    {
        const float sideAngle = Pi - static_cast<float>(i) * sideStep;
        const float radialOffset = minorRadius * std::cos(sideAngle);
        const float y = minorRadius * std::sin(sideAngle);

        for (int j = 0; j <= sectorCount; ++j)
        {
            const float sectorAngle = static_cast<float>(j) * sectorStep;
            const float cosSector = std::cos(sectorAngle);
            const float sinSector = std::sin(sectorAngle);
            const float ringRadius = majorRadius + radialOffset;

            meshData.vertices.push_back(makeVertex(
                { ringRadius * cosSector, y, ringRadius * sinSector },
                {
                    std::cos(sideAngle) * cosSector,
                    std::sin(sideAngle),
                    std::cos(sideAngle) * sinSector
                },
                {
                    static_cast<float>(j) / static_cast<float>(sectorCount),
                    static_cast<float>(i) / static_cast<float>(sideCount)
                }));
        }
    }

    for (int i = 0; i < sideCount; ++i)
    {
        unsigned int currentSide = static_cast<unsigned int>(i * (sectorCount + 1));
        unsigned int nextSide = currentSide + static_cast<unsigned int>(sectorCount + 1);

        for (int j = 0; j < sectorCount; ++j, ++currentSide, ++nextSide)
        {
            // Song Ho's torus is Z-up. Reversing each triangle preserves its
            // outward winding after adapting the coordinates to Y-up.
            meshData.indices.insert(meshData.indices.end(), {
                currentSide, currentSide + 1, nextSide,
                currentSide + 1, nextSide + 1, nextSide
            });
        }
    }

    return meshData;
}
