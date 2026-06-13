#pragma once

#include <system_error>

#include "code_enumerator.hpp"
#include "category.hpp"

namespace std
{
    inline error_code make_error_code(
        net::error::code_enumerator enumerator) noexcept
    {
        return error_code {
            static_cast<int>(enumerator),
            net::error::category::instantiation()
        };
    }
}
