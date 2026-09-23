#pragma once

#include <system_error>
#include <type_traits>

#include <cerrno>

namespace net::error
{
    enum class code_enumerator
    {
        address_is_already_in_use         = EADDRINUSE,
        broken_pipe                       = EPIPE,
        connection_refused                = ECONNREFUSED,
        destination_address_required      = EDESTADDRREQ,
        invalid_ipv4_address,
        listen_operation_is_not_supported = EOPNOTSUPP,

#if EAGAIN == EWOULDBLOCK

        resource_unavailable_try_again    = EAGAIN,

#else

        operation_would_block             = EWOULDBLOCK,
        resource_unavailable_try_again    = EAGAIN,

#endif

        socket_is_already_bound,
        socket_is_already_connected       = EISCONN,
        socket_is_closed                  = EBADF,
        socket_is_not_bound,
        socket_is_not_connected           = ENOTCONN,
        socket_is_not_listening,
        success                           = 0
    };
}

namespace std
{
    template<> struct is_error_code_enum<net::error::code_enumerator> final :
        public true_type
    {};
}
