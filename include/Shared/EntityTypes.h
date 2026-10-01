#pragma once

#include <cstdint>
#include <string>
#include <utility>

using EntityId = std::uint64_t;
constexpr EntityId InvalidEntityId = 0;

struct EntityInfo
{
    EntityId id = InvalidEntityId;
    std::string displayName;

    void setInfo(std::string name, EntityId newId)
    {
        id = newId;
        displayName = std::move(name);
    }
};
