#pragma once

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

        socket(const net::generic::basic_endpoint& endpoint)
        {
            open();

            bind(endpoint);
        }

        socket(
            std::error_code&                    error,
            const net::generic::basic_endpoint& endpoint) noexcept
        {
            if (open(error); error)
            {
                return;
            }

            bind(error, endpoint);
        }

        socket(socket&& other) noexcept(
            std::is_nothrow_move_constructible_v<
                net::generic::basic_datagram_socket>) :
                    net::generic::basic_datagram_socket {std::move(other)}
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
