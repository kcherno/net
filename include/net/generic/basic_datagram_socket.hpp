#pragma once

#include <system_error>
#include <string_view>
#include <type_traits>
#include <utility>
#include <string>

#include <cerrno>

#include <sys/socket.h>

#include "net/debug/throw_exception.hpp"

#include "basic_socket.hpp"

namespace net::generic
{
    template<name_requirement::Protocol T>
    class basic_datagram_socket final : public basic_socket<T>
    {
    public:

        using endpoint_type = typename basic_socket<T>::endpoint_type;

        basic_datagram_socket() = default;

        basic_datagram_socket(const basic_datagram_socket&) = delete;

        basic_datagram_socket(basic_datagram_socket&& other) noexcept(
            std::is_nothrow_move_constructible_v<basic_socket<T>>) :
                basic_socket<T> {std::move(other)}
        {}

        basic_datagram_socket(const endpoint_type& endpoint) :
            basic_socket<T> {endpoint}
        {}

        basic_datagram_socket(
            std::error_code& error, const endpoint_type& endpoint) noexcept :
                basic_socket<T> {error, endpoint}
        {}

        basic_datagram_socket& operator=(
            const basic_datagram_socket&) = delete;

        basic_datagram_socket& operator=(basic_datagram_socket&& other)
            noexcept(std::is_nothrow_move_assignable_v<basic_socket<T>>)
        {
            basic_socket<T>::operator=(std::move(other));

            return *this;
        }

        void send(std::string_view string, int flags = 0) const
        {
            std::error_code error;

            send(error, string, flags);

            debug::throw_exception(error, __func__);
        }

        void send(
            std::error_code& error,
            std::string_view string,
            int              flags = 0) const noexcept
        {
            if (basic_socket<T>::error_if_socket_is_closed(error))
            {
                return;
            }

            const auto sent_bytes = ::send(
                basic_socket<T>::native_handle(),
                string.data(),
                string.size(),
                flags
            );

            if (sent_bytes == -1)
            {
                error = std::make_error_code(std::errc {errno});
            }
        }

        void send_to(
            const endpoint_type& endpoint,
            std::string_view     string,
            int                  flags = 0) const
        {
            std::error_code error;

            send_to(error, endpoint, string, flags);

            debug::throw_exception(error, __func__);
        }

        void send_to(
            std::error_code&     error,
            const endpoint_type& endpoint,
            std::string_view     string,
            int                  flags = 0) const noexcept
        {
            if (basic_socket<T>::error_if_socket_is_closed(error))
            {
                return;
            }

            const auto sent_bytes = ::sendto(
                basic_socket<T>::native_handle(),
                string.data(),
                string.size(),
                flags,
                endpoint.data(),
                endpoint.size()
            );

            if (sent_bytes == -1)
            {
                error = std::make_error_code(std::errc {errno});
            }
        }
    };
}
