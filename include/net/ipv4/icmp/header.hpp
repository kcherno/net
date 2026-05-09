#pragma once

#include <cstdint>

namespace net::ipv4::icmp
{
    struct header final
    {
        enum class type_enumerator : std::uint8_t
        {

        };

        type_enumerator type;
        std::uint8_t    code;
        std::uint16_t   checksum;
    };
}
