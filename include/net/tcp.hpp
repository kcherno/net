#pragma once

#include <sys/socket.h>

#include "generic/basic_stream_socket.hpp"

#include "protocol_enumerator.hpp"
#include "ipv4.hpp"

namespace net
{
    class tcp final
    {
    public:

        using domain_type = ipv4;
        using endpoint    = domain_type::endpoint;

        tcp() = delete;

        static consteval protocol_enumerator protocol() noexcept
        {
            return protocol_enumerator::tcp;
        }

        static consteval int type() noexcept
        {
            return SOCK_STREAM;
        }

        using socket = generic::basic_stream_socket<tcp>;
    };
}
