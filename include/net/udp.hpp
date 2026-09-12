#pragma once

#include <sys/socket.h>

#include "generic/basic_acceptor.hpp"

#include "protocol_enumerator.hpp"
#include "ipv4.hpp"

namespace net
{
    class udp final
    {
    public:

        using domain_type = ipv4;
        using endpoint    = domain_type::endpoint;

        udp() = delete;

        static consteval protocol_enumerator protocol() noexcept
        {
            return protocol_enumerator::udp;
        }

        static consteval int type() noexcept
        {
            return SOCK_DGRAM;
        }

        using socket = generic::basic_socket<udp>;
    };
}
