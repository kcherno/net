#pragma once

#include <source_location>
#include <system_error>
#include <type_traits>
#include <utility>
#include <string>

#include "net/debug/throw_exception.hpp"

#include "net/detail/make_error_code.hpp"

#include "net/name_requirement/protocol.hpp"

#include "net/error_code_enumerator.hpp"

#include "basic_socket.hpp"

namespace net::generic
{
    template<name_requirement::Protocol T>
    class basic_stream_socket final : public basic_socket<T>
    {
    public:

        using endpoint_type = typename basic_socket<T>::endpoint_type;

        using basic_socket<T>::is_open;
        using basic_socket<T>::native_handler;

        basic_stream_socket() = default;

        basic_stream_socket(const basic_stream_socket&) = delete;

        basic_stream_socket(basic_stream_socket&& other) noexcept(
            std::is_nothrow_move_constructible_v<basic_socket<T>>) :
                basic_socket<T> {std::move(other)}
        {}

        basic_stream_socket(const endpoint_type& endpoint) :
            basic_socket<T> {endpoint}
        {}

        basic_stream_socket(
            std::error_code& error, const endpoint_type& endpoint) noexcept :
                basic_socket<T> {error, endpoint}
        {}

        basic_stream_socket& operator=(const basic_stream_socket&) = delete;

        basic_stream_socket& operator=(basic_stream_socket&& other)
            noexcept(std::is_nothrow_move_assignable_v<basic_socket<T>>)
        {
            basic_socket<T>::operator=(std::move(other));

            return *this;
        }

        void receive(std::string& string, int flags = 0) const
        {
            std::error_code error;

            receive(error, string, flags);

            debug::throw_exception(
                error, std::source_location::current().function_name());
        }

        void receive(
            std::error_code& error,
            std::string&     string,
            int              flags = 0) const noexcept
        {
            if (is_open())
            {
                const auto string_size_before_receiving = string.size();

                string.resize(string.capacity());

                const auto received_bytes = ::recv(
                    native_handler(), string.data(), string.capacity(), flags);

                if (received_bytes == -1)
                {
                    error = std::make_error_code(
                        error_code_enumerator {errno});
                }

                else
                {
                    error.clear();
                }

                string.resize(received_bytes == -1 ?
                    string_size_before_receiving : received_bytes);
            }

            else
            {
                error = std::make_error_code(
                    error_code_enumerator::socket_is_closed);
            }
        }

        void send(std::string_view string, int flags = 0) const
        {
            std::error_code error;

            send(error, string, flags);

            debug::throw_exception(
                error, std::source_location::current().function_name());
        }

        void send(
            std::error_code& error,
            std::string_view string,
            int              flags = 0) const noexcept
        {
            if (is_open())
            {
                const auto sent_bytes = ::send(
                    native_handler(), string.data(), string.size(), flags);

                if (sent_bytes == -1)
                {
                    error = std::make_error_code(
                        error_code_enumerator {errno});
                }

                else
                {
                    error.clear();
                }
            }

            else
            {
                error = std::make_error_code(
                    error_code_enumerator::socket_is_closed);
            }
        }
    };
}
