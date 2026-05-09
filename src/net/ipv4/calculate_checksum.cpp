#include <string_view>

#include <cstdint>

#include "net/ipv4/calculate_checksum.hpp"

using namespace net::ipv4;

std::uint16_t calculate_checksum(std::string_view string) noexcept
{
    std::uint32_t checksum;

    auto data = string.data();
    auto size = string.size();

    for (; size > 1; size -=2, data += 2)
    {
        checksum += *reinterpret_cast<std::uint16_t*>(data);
    }

    if (size)
    {
        checksum += *data;
    }

    while (checksum >> 16)
    {
        checksum = (checksum & 0xFFFF) + (checksum >> 16);
    }

    return ~(static_cast<std::uint16_t>(checksum));
}
