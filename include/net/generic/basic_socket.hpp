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

#include "net/debug/throw_exception.hpp"

#include "net/error/error.hpp"

#include "basic_endpoint.hpp"

namespace net::generic
{
    class basic_socket
    {
    public:

        using native_handler_type = int;

        basic_socket() = default;

        basic_socket(const basic_socket&) = delete;

        basic_socket(basic_socket&& other) noexcept(
            std::is_nothrow_move_constructible_v<
                std::optional<native_handler_type>>) :
                    socket_ {std::move(other.socket_)}
        {
            other.socket_.reset();
        }

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

        void connect(const basic_endpoint& endpoint) const
        {
            std::error_code error;

            connect(error, endpoint);

            debug::throw_exception(error, __func__);
        }

        void connect(
            std::error_code&      error,
            const basic_endpoint& endpoint) const noexcept
        {
            if (error_if_socket_is_closed(error))
            {
                return;
            }

            const auto result = ::connect(
                native_handler(), endpoint.data(), endpoint.size());

            if (result == -1)
            {
                error = std::make_error_code(
                    error::code_enumerator::socket_is_closed);
            }
        }

        void bind(const basic_endpoint& endpoint) const
        {
            std::error_code error;

            bind(error, endpoint);

            debug::throw_exception(error, __func__);
        }

        void bind(
            std::error_code&      error,
            const basic_endpoint& endpoint) const noexcept
        {
            if (error_if_socket_is_closed(error))
            {
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
            }

            socket_.reset();
        }

        virtual constexpr int domain() const noexcept = 0;

        void endpoint(basic_endpoint& endpoint) const
        {
            std::error_code error;

            this->endpoint(error, endpoint);

            debug::throw_exception(error, __func__);
        }

        void endpoint(
            std::error_code& error,
            basic_endpoint&  endpoint) const noexcept
        {
            if (error_if_socket_is_closed(error))
            {
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
            std::error_code error;

            if (error_if_socket_is_closed(error))
            {
                debug::throw_exception(error, __func__);
            }

            return socket_.value();
        }

        void open()
        {
            std::error_code error;

            open(error);

            debug::throw_exception(error, __func__);
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

        void remote_endpoint(basic_endpoint& endpoint) const
        {
            std::error_code error;

            remote_endpoint(error, endpoint);

            debug::throw_exception(error, __func__);
        }

        void remote_endpoint(
            std::error_code& error,
            basic_endpoint&  endpoint) const noexcept
        {
            if (error_if_socket_is_closed(error))
            {
                return;
            }

            auto endpoint_size = endpoint.size();

            const auto result = ::getpeername(
                native_handler(), endpoint.data(), &endpoint_size);

            if (result == -1)
            {
                error = std::make_error_code(std::errc {errno});
            }
        }

        virtual constexpr int type() const noexcept = 0;

    protected:

        bool error_if_socket_is_closed(std::error_code& error) const noexcept
        {
            if (not is_open())
            {
                error = std::make_error_code(
                    error::code_enumerator::socket_is_closed);
            }

            return static_cast<bool>(error);
        }

    private:

        std::optional<native_handler_type> socket_;
    };
}
