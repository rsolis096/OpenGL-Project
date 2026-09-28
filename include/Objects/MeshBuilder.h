#pragma once

#include "Objects/MeshData.h"

enum class PrimitiveKind
{
    Cube,
    Sphere,
    Plane,
    Count
};

class MeshBuilder {

public:

    static MeshData build(PrimitiveKind kind);

private:

    static MeshData buildCubeMesh();
    static MeshData buildSphereMesh();
    static MeshData buildPlaneMesh();
};
