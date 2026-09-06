#pragma once

#include <source_location>
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

        using protocol_type       = T;
        using domain_type         = protocol_type::domain_type;
        using endpoint_type       = domain_type::endpoint;
        using native_handle_type = int;

        basic_socket(const basic_socket&) = delete;

        basic_socket(basic_socket&& other) noexcept(
            std::is_nothrow_move_assignable_v<basic_socket>) :
                basic_socket {}
        {
            this->operator=(std::move(other));
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
            std::is_nothrow_swappable_v<std::optional<endpoint_type>> &&
            std::is_nothrow_swappable_v<std::optional<native_handle_type>>)
        {
            if (this != &other)
            {
                std::swap(endpoint_,        other.endpoint_);
                std::swap(remote_endpoint_, other.remote_endpoint_);
                std::swap(socket_,          other.socket_);
            }

            return *this;
        }

        ~basic_socket()
        {
            close();
        }

        void bind(const endpoint_type& endpoint)
        {
            std::error_code error;

            bind(error, endpoint);

            debug::throw_exception(
                error, std::source_location::current().function_name());
        }

        void bind(
            std::error_code& error, const endpoint_type& endpoint) noexcept(
                std::is_nothrow_assignable_v<
                    std::optional<endpoint_type>, endpoint_type>)
        {
            if (is_open())
            {
                if (is_bound())
                {
                    error = std::make_error_code(
                        error_code_enumerator::socket_is_already_bound);
                }

                else
                {
                    const int result = ::bind(
                        socket_.value(), endpoint.data(), endpoint.size());

                    if (result == -1)
                    {
                        error = std::make_error_code(
                            error_code_enumerator {errno});
                    }

                    else
                    {
                        endpoint_ = endpoint;

                        error.clear();
                    }
                }
            }

            else
            {
                error = std::make_error_code(
                    error_code_enumerator::socket_is_closed);
            }
        }

        void connect(const endpoint_type& endpoint)
        {
            std::error_code error;

            connect(error, endpoint);

            debug::throw_exception(
                error, std::source_location::current().function_name());
        }

        void connect(
            std::error_code&     error,
            const endpoint_type& endpoint) noexcept(
                std::is_nothrow_assignable_v<
                    std::optional<endpoint_type>, endpoint_type>)
        {
            if (is_open())
            {
                if (is_connected())
                {
                    error = std::make_error_code(
                        error_code_enumerator::socket_is_already_connected);
                }

                else
                {
                    const int result = ::connect(
                        socket_.value(), endpoint.data(), endpoint.size());

                    if (result == -1)
                    {
                        error = std::make_error_code(
                            error_code_enumerator {errno});
                    }

                    else
                    {
                        remote_endpoint_ = endpoint;

                        error.clear();
                    }
                }
            }

            else
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

            endpoint_.reset();

            remote_endpoint_.reset();

            socket_.reset();
        }

        static constexpr int domain() noexcept
        {
            return domain_type::domain();
        }

        const endpoint_type& endpoint() const &
        {
            if (not is_bound())
            {
                const auto error = std::make_error_code(
                    error_code_enumerator::socket_is_not_bound);

                debug::throw_exception(
                    error, std::source_location::current().function_name());
            }

            return endpoint_.value();
        }

        const std::optional<endpoint_type>&
        endpoint(std::error_code& error) const & noexcept
        {
            if (is_bound())
            {
                error.clear();
            }

            else
            {
                error = std::make_error_code(
                    error_code_enumerator::socket_is_not_bound);
            }

            return endpoint_;
        }

        constexpr bool is_bound() const noexcept
        {
            return endpoint_.has_value();
        }

        constexpr bool is_connected() const noexcept
        {
            return remote_endpoint_.has_value();
        }

        constexpr bool is_open() const noexcept
        {
            return socket_.has_value();
        }

        const native_handle_type& native_handle() const &
        {
            if (not is_open())
            {
                const auto error = std::make_error_code(
                    error_code_enumerator::socket_is_closed);

                debug::throw_exception(
                    error, std::source_location::current().function_name());
            }

            return socket_.value();
        }

        const std::optional<native_handle_type>&
        native_handle(std::error_code& error) const & noexcept
        {
            if (is_open())
            {
                error.clear();
            }

            else
            {
                error = std::make_error_code(
                    error_code_enumerator::socket_is_closed);
            }

            return socket_;
        }

        void open()
        {
            std::error_code error;

            open(error);

            debug::throw_exception(
                error, std::source_location::current().function_name());
        }

        void open(std::error_code& error) noexcept
        {
            const native_handle_type socket = ::socket(
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

                error.clear();
            }
        }

        static constexpr protocol_enumerator protocol() noexcept
        {
            return protocol_type::protocol();
        }

        const endpoint_type& remote_endpoint() const &
        {
            if (not is_connected())
            {
                const auto error = std::make_error_code(
                    error_code_enumerator::socket_is_not_connected);

                debug::throw_exception(
                    error, std::source_location::current().function_name());
            }

            return remote_endpoint_.value();
        }

        const std::optional<endpoint_type>&
        remote_endpoint(std::error_code& error) const & noexcept
        {
            if (is_connected())
            {
                error.clear();
            }

            else
            {
                error = std::make_error_code(
                    error_code_enumerator::socket_is_not_connected);
            }

            return remote_endpoint_;
        }

        static constexpr int type() noexcept
        {
            return protocol_type::type();
        }

    protected:

        basic_socket() = default;

    private:

        std::optional<endpoint_type>      endpoint_;
        std::optional<endpoint_type>      remote_endpoint_;
        std::optional<native_handle_type> socket_;
    };
}
