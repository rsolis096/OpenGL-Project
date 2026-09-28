#pragma once

#include "Objects/MeshData.h"

enum class PrimitiveKind
{
    Cube,
    Sphere,
    Plane,
    Cylinder,
    Cone,
    Torus,
    Count
};

class MeshBuilder {

public:

    static MeshData build(PrimitiveKind kind);

private:

    static MeshData buildCubeMesh();
    static MeshData buildSphereMesh();
    static MeshData buildPlaneMesh();
    static MeshData buildCylinderMesh();
    static MeshData buildConeMesh();
    static MeshData buildTorusMesh();
};
