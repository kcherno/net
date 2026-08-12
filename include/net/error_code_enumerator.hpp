#pragma once

#include <system_error>
#include <type_traits>

#include <cerrno>

namespace net
{
    enum class error_code_enumerator
    {
        success            = 0,
        connection_refused = ECONNREFUSED,
        invalid_ipv4_address,
        socket_is_closed   = EBADF
    };
}

namespace std
{
    template<>
    struct is_error_code_enum<net::error_code_enumerator> final : true_type {};
}
