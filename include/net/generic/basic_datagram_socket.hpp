#pragma once

#include <system_error>
#include <string_view>
#include <type_traits>
#include <utility>
#include <string>

#include <cerrno>

#include <sys/socket.h>

#include "basic_endpoint.hpp"
#include "basic_socket.hpp"

namespace net::generic
{
    class basic_datagram_socket : public basic_socket
    {
    public:

        basic_datagram_socket() = default;

        basic_datagram_socket(const basic_datagram_socket&) = delete;

        basic_datagram_socket(basic_datagram_socket&& other) noexcept(
            std::is_nothrow_move_constructible_v<basic_socket>) :
                basic_socket {std::move(other)}
        {}

        basic_datagram_socket& operator=(
            const basic_datagram_socket&) = delete;

        basic_datagram_socket& operator=(basic_datagram_socket&& other)
            noexcept(std::is_nothrow_move_assignable_v<basic_socket>)
        {
            basic_socket::operator=(std::move(other));

            return *this;
        }

        virtual ~basic_datagram_socket() = default;

        void receive_from(
            basic_endpoint& endpoint,
            std::string&    string,
            int             flags = 0)
        {
            std::error_code error;

            if (receive_from(error, endpoint, string, flags); error)
            {
                throw std::system_error {
                    error, "basic_datagram_socket::receive_from"
                };
            }
        }

        void receive_from(
            std::error_code& error,
            basic_endpoint&  endpoint,
            std::string&     string,
            int              flags = 0) noexcept
        {
            if (not is_open())
            {
                error = std::make_error_code(std::errc::bad_file_descriptor);

                return;
            }

            auto endpoint_size = endpoint.size();

            const auto receive_bytes = ::recvfrom(
                native_handler(),
                string.data(),
                string.capacity(),
                flags,
                endpoint.data(),
                &endpoint_size);

            if (receive_bytes == -1)
            {
                error = std::make_error_code(std::errc {errno});

                return;
            }

            string.resize(receive_bytes);
        }

        void send_to(
            const basic_endpoint& endpoint,
            std::string_view      string,
            int                   flags = 0)
        {
            std::error_code error;

            if (send_to(error, endpoint, string, flags); error)
            {
                throw std::system_error {
                    error, "basic_datagram_socket::send_to"
                };
            }
        }

        void send_to(
            std::error_code&      error,
            const basic_endpoint& endpoint,
            std::string_view      string,
            int                   flags = 0) noexcept
        {
            if (not is_open())
            {
                error = std::make_error_code(std::errc::bad_file_descriptor);

                return;
            }

            const auto sent_bytes = ::sendto(
                native_handler(),
                string.data(),
                string.size(),
                flags,
                endpoint.data(),
                endpoint.size());

            if (sent_bytes == -1)
            {
                error = std::make_error_code(std::errc {errno});
            }
        }

        constexpr int type() const noexcept override
        {
            return SOCK_DGRAM;
        }
    };
}
