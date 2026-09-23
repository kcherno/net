#include <string>

#include <cerrno>

#include "net/error/code_enumerator.hpp"
#include "net/error/category.hpp"

std::string net::error::category::message(int code) const
{
    using enum code_enumerator;

    switch (code_enumerator {code})
    {
        case address_is_already_in_use:
            return "address is already in use";

        case broken_pipe:
            return "broken pipe";

        case connection_refused:
            return "no socket is listening on the target address";

        case destination_address_required:
            return "destination address required";

        case invalid_ipv4_address:
            return "invalid ipv4 address";

        case listen_operation_is_not_supported:
            return "protocol does not support the listen operation";

#if EAGAIN == EWOULDBLOCK

        case resource_unavailable_try_again:
            return "socket is in non-blocking mode and "
                    "the data is not yet ready";

#else

        case operation_would_block:
            return "socket is in non-blocking mode and "
                    "the operation would block";

        case resource_unavailable_try_again:
            return "socket is in non-blocking mode and "
                   "the data is not yet ready";

#endif

        case socket_is_already_bound:
            return "socket is already bound";

        case socket_is_already_connected:
            return "socket is already connected";

        case socket_is_closed:
            return "socket is closed";

        case socket_is_not_bound:
            return "socket is not bound";

        case socket_is_not_connected:
            return "socket is not connected";

        case socket_is_not_listening:
            return "socket is not listening";

        case success:
            return "success";
        }

    return "undefined error";
}
