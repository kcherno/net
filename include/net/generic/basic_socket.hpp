#pragma once

#include <system_error>
#include <type_traits>
#include <stdexcept>
#include <optional>
#include <utility>
#include <memory>

#include <cerrno>

#include <sys/socket.h>

#include <unistd.h>

#include "basic_endpoint.hpp"

namespace net::generic
{
    class basic_socket
    {
    public:

        using native_handler_type = int;

        basic_socket() = default;

        basic_socket(const basic_endpoint& endpoint)
        {
            open();

            bind(endpoint);
        }

        basic_socket(
            std::error_code& error, const basic_endpoint& endpoint) noexcept
        {
            if (open(error); error)
            {
                return;
            }

            bind(error, endpoint);
        }

        basic_socket(const basic_socket&) = delete;

        basic_socket(basic_socket&& other) noexcept(
            std::is_nothrow_move_constructible_v<
                std::optional<native_handler_type>>) :
                    socket_ {std::move(other.socket_)}
        {}

        basic_socket& operator=(const basic_socket&) = delete;

        basic_socket& operator=(basic_socket&& other) noexcept(
            std::is_nothrow_move_assignable_v<
                std::optional<native_handler_type>>)
        {
            if (this != &other)
            {
                std::swap(socket_, other.socket_);
            }

            return *this;
        }

        virtual ~basic_socket()
        {
            close();
        }

        void bind(const basic_endpoint& endpoint) const
        {
            std::error_code error;

            if (bind(error, endpoint); error)
            {
                throw std::system_error {error, "basic_socket::bind"};
            }
        }

        void bind(
            std::error_code&      error,
            const basic_endpoint& endpoint) const noexcept
        {
            if (not is_open())
            {
                error = std::make_error_code(
                    std::errc::bad_file_descriptor);

                return;
            }

            const auto result = ::bind(
                native_handler(), endpoint.data(), endpoint.size());

            if (result == -1)
            {
                error = std::make_error_code(std::errc {errno});
            }
        }

        void close() noexcept
        {
            if (is_open())
            {
                ::close(socket_.value());

                socket_.reset();
            }
        }

        virtual constexpr int domain() const noexcept = 0;

        void endpoint(basic_endpoint& endpoint) const
        {
            std::error_code error;

            if (this->endpoint(error, endpoint); error)
            {
                throw std::system_error {error, "basic_socket::endpoint"};
            }
        }

        void endpoint(
            std::error_code& error,
            basic_endpoint&  endpoint) const noexcept
        {
            if (not is_open())
            {
                error = std::make_error_code(
                    std::errc::bad_file_descriptor);

                return;
            }

            auto endpoint_size = endpoint.size();

            const auto result = ::getsockname(
                native_handler(), endpoint.data(), &endpoint_size);

            if (result == -1)
            {
                error = std::make_error_code(std::errc {errno});
            }
        }

        constexpr bool is_open() const noexcept
        {
            return socket_.has_value();
        }

        const native_handler_type& native_handler() const
        {
            if (not is_open())
            {
                throw std::system_error {
                    std::make_error_code(std::errc::bad_file_descriptor),
                    "basic_socket::native_handler"
                };
            }

            return socket_.value();
        }

        void open()
        {
            std::error_code error;

            if (open(error); error)
            {
                throw std::system_error {error, "basic_socket::open"};
            }
        }

        void open(std::error_code& error) noexcept
        {
            const auto socket = ::socket(
                domain(), type() | SOCK_CLOEXEC, protocol());

            if (socket == -1)
            {
                error = std::make_error_code(std::errc {errno});
            }

            else
            {
                close();

                socket_ = socket;
            }
        }

        virtual constexpr int protocol() const noexcept = 0;

        virtual constexpr int type() const noexcept = 0;

    private:

        std::optional<native_handler_type> socket_;
    };
}
