#pragma once

#include <string_view>
#include <exception>

namespace net::test
{
    inline constexpr bool
    accept_through_closed_socket(const std::exception& exception) noexcept
    {
        return std::string_view(exception.what())
            .ends_with("socket is closed");
    }

    inline constexpr bool accept_through_non_listening_socket(
        const std::exception& exception) noexcept
    {
        return std::string_view(exception.what())
            .ends_with("socket is not listening");
    }

    inline constexpr bool
    accept_through_unbound_socket(const std::exception& exception) noexcept
    {
        return std::string_view(exception.what())
            .ends_with("socket is not bound");
    }

    inline constexpr bool
    bind_through_already_bound_socket(const std::exception& exception) noexcept
    {
        return std::string_view(exception.what())
            .ends_with("socket is already bound");
    }

    inline constexpr bool
    bind_through_closed_socket(const std::exception& exception) noexcept
    {
        return std::string_view(exception.what())
            .ends_with("socket is closed");
    }

    inline constexpr bool
    connect_through_closed_socket(const std::exception& exception) noexcept
    {
        return std::string_view(exception.what())
            .ends_with("socket is closed");
    }

    inline constexpr bool
    connect_to_non_listening_socket(const std::exception& exception) noexcept
    {
        return std::string_view(exception.what())
            .ends_with("no socket is listening on the target address");
    }

    inline constexpr bool get_endpoint_through_unbound_socket(
        const std::exception& exception) noexcept
    {
        return std::string_view(exception.what())
            .ends_with("socket is not bound");
    }

    inline constexpr bool get_native_handle_through_closed_socket(
        const std::exception& exception) noexcept
    {
        return std::string_view(exception.what())
            .ends_with("socket is closed");
    }

    inline constexpr bool get_remote_endpoint_through_unconnected_socket(
        const std::exception& exception) noexcept
    {
        return std::string_view(exception.what())
            .ends_with("socket is not connected");
    }

    inline constexpr bool listen_operation_is_not_supported(
        const std::exception& exception) noexcept
    {
        return std::string_view(exception.what())
            .ends_with("protocol does not support the listen operation");
    }

    inline constexpr bool listen_through_closed_socket(
        const std::exception& exception) noexcept
    {
        return std::string_view(exception.what())
            .ends_with("socket is closed");
    }

    inline constexpr bool receive_through_closed_socket(
        const std::exception& exception) noexcept
    {
        return std::string_view(exception.what())
            .ends_with("socket is closed");
    }

    inline constexpr bool receive_through_unconnected_socket(
        const std::exception& exception) noexcept
    {
        return std::string_view(exception.what())
            .ends_with("socket is not connected");
    }

    inline constexpr bool send_through_closed_socket(
        const std::exception& exception) noexcept
    {
        return std::string_view(exception.what())
            .ends_with("socket is closed");
    }

    inline constexpr bool send_through_unconnected_stream_socket(
        const std::exception& exception) noexcept
    {
        return std::string_view(exception.what())
            .ends_with("broken pipe");
    }

    inline constexpr bool send_through_unconnected_datagram_socket(
        const std::exception& exception) noexcept
    {
        return std::string_view(exception.what())
            .ends_with("destination address required");
    }
}
