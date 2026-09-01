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
        using native_handler_type = int;

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
            std::is_nothrow_move_assignable_v<
                std::optional<native_handler_type>>)
        {
            if (this != &other)
            {
                std::swap(socket_,   other.socket_);
                std::swap(is_bound_, other.is_bound_);
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
            std::error_code& error, const endpoint_type& endpoint) noexcept
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
                        error.clear();

                        is_bound_ = true;
                    }
                }
            }

            else
            {
                error = std::make_error_code(
                    error_code_enumerator::socket_is_closed);
            }
        }

        void connect(const endpoint_type& endpoint) const
        {
            std::error_code error;

            connect(error, endpoint);

            debug::throw_exception(
                error, std::source_location::current().function_name());
        }

        void connect(
            std::error_code&     error,
            const endpoint_type& endpoint) const noexcept
        {
            if (is_open())
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
                    error.clear();
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

            socket_.reset();

            is_bound_ = false;
        }

        static constexpr int domain() noexcept
        {
            return domain_type::domain();
        }

        endpoint_type endpoint() const
        {
            std::error_code error;

            auto result = endpoint(error);

            debug::throw_exception(
                error, std::source_location::current().function_name());

            return result.value();
        }

        std::optional<endpoint_type>
        endpoint(std::error_code& error) const noexcept
        {
            if (is_open())
            {
                endpoint_type endpoint;

                auto size = endpoint.size();

                const auto result = ::getsockname(
                    socket_.value(), endpoint.data(), &size);

                if (result == -1)
                {
                    error = std::make_error_code(
                        error_code_enumerator {errno});

                    return std::nullopt;
                }

                else
                {
                    error.clear();

                    return endpoint;
                }
            }

            error = std::make_error_code(
                error_code_enumerator::socket_is_closed);

            return std::nullopt;
        }

        constexpr bool is_bound() const noexcept
        {
            return is_bound_;
        }

        constexpr bool is_open() const noexcept
        {
            return socket_.has_value();
        }

        const native_handler_type& native_handler() const
        {
            if (not is_open())
            {
                debug::throw_exception(
                    std::make_error_code(
                        error_code_enumerator::socket_is_closed),
                    std::source_location::current().function_name()
                );
            }

            return socket_.value();
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
                error.clear();

                close();

                socket_ = socket;
            }
        }

        static constexpr protocol_enumerator protocol() noexcept
        {
            return protocol_type::protocol();
        }

        endpoint_type remote_endpoint() const
        {
            std::error_code error;

            auto result = remote_endpoint(error);

            debug::throw_exception(
                error, std::source_location::current().function_name());

            return result.value();
        }

        std::optional<endpoint_type>
        remote_endpoint(std::error_code& error) const noexcept
        {
            if (is_open())
            {
                endpoint_type endpoint;

                auto size = endpoint.size();

                const auto result = ::getpeername(
                    socket_.value(), endpoint.data(), &size);

                if (result == -1)
                {
                    error = std::make_error_code(
                        error_code_enumerator {errno});

                    return std::nullopt;
                }

                else
                {
                    error.clear();

                    return endpoint;
                }
            }

            error = std::make_error_code(
                error_code_enumerator::socket_is_closed);

            return std::nullopt;
        }

        static constexpr int type() noexcept
        {
            return protocol_type::type();
        }

    protected:

        constexpr basic_socket() noexcept :
            socket_   {},
            is_bound_ {false}
        {}

    private:

        std::optional<native_handler_type> socket_;
        bool                               is_bound_;
    };
}
