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

    inline constexpr bool native_handler_through_closed_socket(
        const std::exception& exception) noexcept
    {
        return std::string_view(exception.what())
            .ends_with("socket is closed");
    }

    inline constexpr bool receive_through_closed_socket(
        const std::exception& exception)
    {
        return std::string_view(exception.what())
            .ends_with("socket is closed");
    }

    inline constexpr bool receive_through_non_connected_socket(
        const std::exception& exception)
    {
        return std::string_view(exception.what())
            .ends_with("socket is not connected");
    }

    inline constexpr bool get_remote_endpoint_through_non_connected_socket(
        const std::exception& exception) noexcept
    {
        return std::string_view(exception.what())
            .ends_with("socket is not connected");
    }

    inline constexpr bool send_through_closed_socket(
        const std::exception& exception)
    {
        return std::string_view(exception.what())
            .ends_with("socket is closed");
    }

    inline constexpr bool send_through_non_connected_socket(
        const std::exception& exception)
    {
        return std::string_view(exception.what())
            .ends_with("broken pipe");
    }
}
