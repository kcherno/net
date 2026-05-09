#pragma once

#include <string_view>

#include <cstdint>

namespace net::ipv4
{
    std::uint16_t calculate_checksum(std::string_view) noexcept;
}
