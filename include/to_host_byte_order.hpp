#pragma once

#include <concepts>

#include "to_network_byte_order.hpp"

namespace net
{
    inline constexpr auto to_host_byte_order(
        std::unsigned_integral auto network_byte_order) noexcept
    {
        return to_network_byte_order(network_byte_order);
    }
}
