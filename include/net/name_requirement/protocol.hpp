#pragma once

#include <concepts>

#include "net/protocol_enumerator.hpp"

#include "domain.hpp"

namespace net::name_requirement
{
    template<typename T>
    concept Protocol = requires
    {
        requires Domain<typename T::domain_type>;

        { T::protocol() } noexcept -> std::same_as<protocol_enumerator>;
        { T::type()     } noexcept -> std::same_as<int>;
    };
}
