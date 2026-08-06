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

#include "net/detail/make_error_code.hpp"

#include "net/name_requirement/protocol.hpp"

#include "net/protocol_enumerator.hpp"

namespace net::generic
{
    template<name_requirement::Protocol T>
    class basic_socket
    {
    public:

        using domain_type         = typename T::domain_type;
        using endpoint_type       = typename domain_type::endpoint;
        using native_handler_type = int;

        basic_socket(const basic_socket&) = delete;

        basic_socket(basic_socket&& other) noexcept(
            std::is_nothrow_move_constructible_v<
                std::optional<native_handler_type>>) :
                    socket_ {std::move(other.socket_)}
        {
            other.socket_.reset();
        }

        basic_socket(const endpoint_type& endpoint)
        {
            open();

            connect(endpoint);
        }

        basic_socket(
            std::error_code& error, const endpoint_type& endpoint) noexcept
        {
            if (open(error); error)
            {
                return;
            }

            connect(error, endpoint);
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

        ~basic_socket()
        {
            close();
        }

        void bind(const endpoint_type& endpoint) const
        {
            std::error_code error;

            bind(error, endpoint);

            debug::throw_exception(error, __func__);
        }

        void bind(
            std::error_code&     error,
            const endpoint_type& endpoint) const noexcept
        {
            if (error_if_socket_is_closed(error))
            {
                return;
            }

            const int result = ::bind(
                socket_.value(), endpoint.data(), endpoint.size());

            if (result == -1)
            {
                error = std::make_error_code(std::errc {errno});
            }
        }

        void connect(const endpoint_type& endpoint) const
        {
            std::error_code error;

            connect(error, endpoint);

            debug::throw_exception(error, __func__);
        }

        void connect(
            std::error_code&     error,
            const endpoint_type& endpoint) const noexcept
        {
            if (error_if_socket_is_closed(error))
            {
                return;
            }

            const int result = ::connect(
                socket_.value(), endpoint.data(), endpoint.size());

            if (result == -1)
            {
                error = std::make_error_code(
                    error_code_enumerator::socket_is_closed);
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

        constexpr int domain() const noexcept
        {
            return domain_type::domain();
        }

        endpoint_type endpoint() const
        {
            endpoint_type endpoint;

            this->endpoint(endpoint);

            return endpoint;
        }

        std::optional<endpoint_type>
        endpoint(std::error_code& error) const noexcept
        {
            std::optional<endpoint_type> optional_endpoint {
                endpoint_type {}
            };

            endpoint(error, optional_endpoint.value());

            if (error)
            {
                optional_endpoint.reset();
            }

            return optional_endpoint;
        }

        void endpoint(endpoint_type& endpoint) const
        {
            std::error_code error;

            this->endpoint(error, endpoint);

            debug::throw_exception(error, __func__);
        }

        void endpoint(
            std::error_code& error, endpoint_type& endpoint) const noexcept
        {
            if (error_if_socket_is_closed(error))
            {
                return;
            }

            auto endpoint_size = endpoint.size();

            const int result = ::getsockname(
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
            const native_handler_type socket = ::socket(
                domain(),
                type() | SOCK_CLOEXEC,
                static_cast<int>(protocol())
            );

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

        constexpr protocol_enumerator protocol() const noexcept
        {
            return T::protocol();
        }

        endpoint_type remote_endpoint() const
        {
            endpoint_type remote_endpoint;

            this->remote_endpoint(remote_endpoint);

            return remote_endpoint;
        }

        std::optional<endpoint_type>
        remote_endpoint(std::error_code& error) const noexcept
        {
            std::optional<endpoint_type> optional_remote_endpoint {
                endpoint_type {}
            };

            remote_endpoint(error, optional_remote_endpoint.value());

            if (error)
            {
                optional_remote_endpoint.reset();
            }

            return optional_remote_endpoint;
        }

        void remote_endpoint(endpoint_type& endpoint) const
        {
            std::error_code error;

            remote_endpoint(error, endpoint);

            debug::throw_exception(error, __func__);
        }

        void remote_endpoint(
            std::error_code& error, endpoint_type& endpoint) const noexcept
        {
            if (error_if_socket_is_closed(error))
            {
                return;
            }

            auto endpoint_size = endpoint.size();

            const int result = ::getpeername(
                native_handler(), endpoint.data(), &endpoint_size);

            if (result == -1)
            {
                error = std::make_error_code(std::errc {errno});
            }
        }

        constexpr int type() const noexcept
        {
            return T::type();
        }

    protected:

        basic_socket() = default;

        bool error_if_socket_is_closed(std::error_code& error) const noexcept
        {
            if (not is_open())
            {
                error = std::make_error_code(
                    error_code_enumerator::socket_is_closed);
            }

            return static_cast<bool>(error);
        }

    private:

        std::optional<native_handler_type> socket_;
    };
}
