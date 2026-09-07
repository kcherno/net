#pragma once

#include <source_location>
#include <system_error>
#include <type_traits>
#include <optional>
#include <utility>

#include <cerrno>

#include <sys/socket.h>

#include "net/debug/throw_exception.hpp"

#include "net/detail/make_error_code.hpp"

#include "net/name_requirement/protocol.hpp"

#include "net/error_code_enumerator.hpp"
#include "net/protocol_enumerator.hpp"

#include "basic_stream_socket.hpp"

namespace net::generic
{
    template<name_requirement::Protocol T>
    class basic_acceptor final
    {
    public:

        using protocol_type       = T;
        using domain_type         = protocol_type::domain_type;
        using socket_type         = basic_stream_socket<protocol_type>;
        using endpoint_type       = socket_type::endpoint_type;
        using native_handle_type = socket_type::native_handler_type;

        static constexpr int maximum_queue_size = SOMAXCONN;

        basic_acceptor() = default;

        basic_acceptor(const basic_acceptor&) = delete;

        basic_acceptor(basic_acceptor&& other) noexcept(
            std::is_nothrow_move_constructible_v<socket_type>) :
                socket_ {std::move(other.socket_)}
        {}

        basic_acceptor(
            const endpoint_type& endpoint,
            int                  queue_size = maximum_queue_size)
        {
            open();

            bind(endpoint);

            listen(queue_size);
        }

        basic_acceptor(
            std::error_code&     error,
            const endpoint_type& endpoint,
            int                  queue_size = maximum_queue_size) noexcept
        {
            if (open(error); error)
            {
                return;
            }

            if (bind(error, endpoint); error)
            {
                return;
            }

            listen(error, queue_size);
        }

        basic_acceptor& operator=(const basic_acceptor&) = delete;

        basic_acceptor& operator=(basic_acceptor&& other) noexcept(
            std::is_nothrow_move_assignable_v<socket_type>)
        {
            if (this != &other)
            {
                socket_ = std::move(other.socket_);
            }

            return *this;
        }

        socket_type accept() const
        {
            std::error_code error;

            auto optional = accept(error);

            if (error)
            {
                debug::throw_exception(
                    error, std::source_location::current().function_name());
            }

            return socket_type {std::move(optional.value())};
        }

        std::optional<socket_type>
        accept(std::error_code& error) const noexcept
        {
            if (is_open())
            {
                int result = ::accept(
                    socket_.native_handle(), nullptr, nullptr);

                if (result == -1)
                {
                    error = std::make_error_code(
                        error_code_enumerator {errno});
                }

                else
                {
                    return std::optional<socket_type> {
                        socket_type {std::move(result)}
                    };
                }
            }

            else
            {
                error = std::make_error_code(
                    error_code_enumerator::socket_is_closed);
            }

            return std::nullopt;
        }

        void bind(const endpoint_type& endpoint) const
        {
            socket_.bind(endpoint);
        }

        void bind(
            std::error_code&     error,
            const endpoint_type& endpoint) const noexcept
        {
            socket_.bind(error, endpoint);
        }

        void close() noexcept
        {
            socket_.close();
        }

        consteval int domain() const noexcept
        {
            return domain_type::domain();
        }

        endpoint_type endpoint() const
        {
            return socket_.endpoint();
        }

        std::optional<endpoint_type>
        endpoint(std::error_code& error) const noexcept
        {
            return socket_.endpoint(error);
        }

        constexpr bool is_open() const noexcept
        {
            return socket_.is_open();
        }

        void listen(int queue_size = maximum_queue_size) const
        {
            std::error_code error;

            if (listen(error, queue_size); error)
            {
                debug::throw_exception(
                    error, std::source_location::current().function_name());
            }
        }

        void listen(
            std::error_code& error,
            int              queue_size = maximum_queue_size) const noexcept
        {
            if (is_open())
            {
                const int result = ::listen(
                    socket_.native_handle(), queue_size);

                if (result == -1)
                {
                    error = std::make_error_code(
                        error_code_enumerator {errno});
                }
            }

            else
            {
                error = std::make_error_code(
                    error_code_enumerator::socket_is_closed);
            }
        }

        const native_handle_type& native_handler() const
        {
            return socket_.native_handle();
        }

        void open()
        {
            socket_.open();
        }

        void open(std::error_code& error) noexcept
        {
            socket_.open(error);
        }

        consteval protocol_enumerator protocol() const noexcept
        {
            return protocol_type::protocol();
        }

        consteval int type() const noexcept
        {
            return protocol_type::type();
        }

    private:

        socket_type socket_;
    };
}
