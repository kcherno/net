#pragma once

#include <system_error>
#include <type_traits>
#include <utility>

#include <netinet/in.h>

#include <sys/socket.h>

#include "net/generic/basic_datagram_socket.hpp"
#include "net/generic/basic_endpoint.hpp"

namespace net::ipv4::icmp
{
    class socket final : public net::generic::basic_datagram_socket
    {
    public:

        socket() = default;

        socket(const socket&) = default;

        socket(socket&& other) noexcept(
            std::is_nothrow_move_constructible_v<
                net::generic::basic_datagram_socket>) :
                    net::generic::basic_datagram_socket {std::move(other)}
        {}

        socket(const net::generic::basic_endpoint& endpoint) :
            net::generic::basic_datagram_socket {endpoint}
        {}

        socket(
            std::error_code&                    error,
            const net::generic::basic_endpoint& endpoint) noexcept :
                net::generic::basic_datagram_socket {error, endpoint}
        {}

        socket& operator=(const socket&) = delete;

        socket& operator=(socket&& other) noexcept(
            std::is_nothrow_move_assignable_v<
                net::generic::basic_datagram_socket>)
        {
            net::generic::basic_datagram_socket::operator=(std::move(other));

            return *this;
        }

        constexpr int domain() const noexcept override
        {
            return AF_INET;
        }

        constexpr int protocol() const noexcept override
        {
            return IPPROTO_ICMP;
        }

        constexpr int type() const noexcept override
        {
            return SOCK_RAW;
        }
    };
}
