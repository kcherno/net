#pragma once

#include <system_error>
#include <type_traits>

#include <cerrno>

namespace net
{
    enum class error_code_enumerator
    {
        address_is_already_in_use = EADDRINUSE,
        connection_refused        = ECONNREFUSED,
        invalid_ipv4_address,
        socket_is_closed          = EBADF,
        success                   = 0
    };
}

namespace std
{
    template<>
    struct is_error_code_enum<net::error_code_enumerator> final : true_type {};
}
