#pragma once

#include <system_error>

#include "net/error_code_enumerator.hpp"
#include "net/error_category.hpp"

namespace std
{
    inline error_code make_error_code(
        net::error_code_enumerator enumerator) noexcept
    {
        return error_code {
            static_cast<int>(enumerator), net::error_category::instantiation()
        };
    }
}
