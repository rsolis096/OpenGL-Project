#pragma once

#include "Objects/Vertex.h"

#include <vector>

struct MeshData
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
};
