#pragma once

#include <cstdint>
#include <string>

#include "Material.h"
#include "Transform.h"

using EntityId = std::uint64_t;
using PlayerId = std::uint32_t;

struct EntityInfo {
    EntityId id = 0;
    std::string displayName;

    void setInfo(std::string name, std::uint64_t id)
    {
        this->id = id;
        displayName = name;
    }
};

enum class Authority {
    Local,
    Host,
    Remote
};

enum class ObjectType
{
    Cube,
    Model,
    Sphere,
    Plane,
    Object,
};

struct NetworkIdentity {
    PlayerId owner = 0;
    Authority authority = Authority::Local;
    bool replicateTransform = true;
};