#pragma once

#include <concepts>

#include "endpoint.hpp"

namespace net::name_requirement
{
    template<typename T>
    concept Domain = requires
    {
        requires Endpoint<typename T::endpoint>;

        { T::domain() } noexcept -> std::same_as<int>;
    };
}
