#pragma once

#include <concepts>

#include "to_network_byte_order.hpp"

namespace net
{
    inline constexpr auto to_host_byte_order(
        std::unsigned_integral auto number) noexcept
    {
        return to_network_byte_order(number);
    }
}
