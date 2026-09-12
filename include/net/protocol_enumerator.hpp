#pragma once

#include <cstdint>

namespace net
{
    enum class protocol_enumerator : std::uint8_t
    {
        undefined = 0,
        icmp      = 1,
        tcp       = 6,
        udp       = 17
    };
}
