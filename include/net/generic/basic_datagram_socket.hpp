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
    };
}
