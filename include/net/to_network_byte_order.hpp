#pragma once

#include <concepts>
#include <bit>

namespace net
{
    inline constexpr auto to_network_byte_order(
        std::unsigned_integral auto number) noexcept
    {
        if constexpr (std::endian::native == std::endian::little)
        {
            return std::byteswap(number);
        }

        else
        {
            return number;
        }
    }
}
