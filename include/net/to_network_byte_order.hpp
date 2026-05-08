#pragma once

#include <concepts>
#include <bit>

namespace net
{
    inline constexpr auto to_network_byte_order(
        std::unsigned_integral auto host_byte_order) noexcept
    {
        if constexpr (std::endian::native == std::endian::little)
        {
            return std::byteswap(host_byte_order);
        }

        else
        {
            return host_byte_order;
        }
    }
}
