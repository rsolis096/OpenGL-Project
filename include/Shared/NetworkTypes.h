#pragma once

#include <cstdint>

using ClientId = std::uint32_t;
using PlayerId = ClientId;

using NetworkEntityId = std::uint64_t;
constexpr NetworkEntityId InvalidNetworkEntityId = 0;

enum class Authority : std::uint8_t
{
    Local,
    Host,
    Remote
};

struct NetworkIdentity
{
    NetworkEntityId entityId = InvalidNetworkEntityId;
    ClientId owner = 0;
    Authority authority = Authority::Local;
    bool replicateTransform = true;
};
