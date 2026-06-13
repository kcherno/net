#pragma once

#include <system_error>
#include <type_traits>

#include <cerrno>

namespace net::error
{
    enum class code_enumerator
    {
        socket_is_closed = EBADF
    };
}

namespace std
{
    template<>
    struct is_error_code_enum<net::error::code_enumerator> final : true_type
    {

    };
}
