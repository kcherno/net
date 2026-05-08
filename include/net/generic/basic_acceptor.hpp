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

#include "basic_socket.hpp"

namespace net::generic
{
    template<name_requirement::Protocol T>
    class basic_acceptor final
    {
    public:

        using protocol_type      = T;
        using domain_type        = protocol_type::domain_type;
        using socket_type        = basic_socket<protocol_type>;
        using endpoint_type      = socket_type::endpoint_type;
        using native_handle_type = socket_type::native_handle_type;

        static constexpr int maximum_queue_size = SOMAXCONN;

        constexpr basic_acceptor() noexcept :
            socket_       {},
            is_listening_ {false}
        {}

        basic_acceptor(const basic_acceptor&) = delete;

        basic_acceptor(basic_acceptor&& other) noexcept(
            std::is_nothrow_move_assignable_v<basic_acceptor>) :
                basic_acceptor {}
        {
            this->operator=(std::move(other));
        }

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
                std::swap(socket_,       other.socket_);
                std::swap(is_listening_, other.is_listening_);
            }

            return *this;
        }

        socket_type accept() const
        {
            std::error_code error;

            auto optional = accept(error);

            debug::throw_exception(
                error, std::source_location::current().function_name());

            return socket_type {std::move(optional.value())};
        }

        std::optional<socket_type>
        accept(std::error_code& error) const noexcept
        {
            if (not is_open())
            {
                error = std::make_error_code(
                    error_code_enumerator::socket_is_closed);

                return std::nullopt;
            }

            if (not is_bound())
            {
                error = std::make_error_code(
                    error_code_enumerator::socket_is_not_bound);

                return std::nullopt;
            }

            if (not is_listening())
            {
                error = std::make_error_code(
                    error_code_enumerator::socket_is_not_listening);

                return std::nullopt;
            }

            endpoint_type remote_endpoint;

            auto remote_endpoint_size = remote_endpoint.size();

            auto result = ::accept(
                socket_.native_handle(),
                remote_endpoint.data(),
                &remote_endpoint_size
            );

            if (result == -1)
            {
                error = std::make_error_code(
                    error_code_enumerator {errno});

                return std::nullopt;
            }

            endpoint_type endpoint;

            auto endpoint_size = endpoint.size();

            ::getsockname(result, endpoint.data(), &endpoint_size);

            socket_type socket;

            socket.endpoint_        = std::move(endpoint);
            socket.remote_endpoint_ = std::move(remote_endpoint);
            socket.socket_          = result;

            return std::optional<socket_type> {
                socket_type {std::move(socket)}
            };
        }

        void bind(const endpoint_type& endpoint)
        {
            socket_.bind(endpoint);
        }

        void bind(
            std::error_code& error, const endpoint_type& endpoint) noexcept(
                noexcept(std::declval<socket_type>().bind(error, endpoint)))
        {
            socket_.bind(error, endpoint);
        }

        void close() noexcept
        {
            socket_.close();

            is_listening_ = false;
        }

        static consteval int domain() noexcept
        {
            return domain_type::domain();
        }

        auto&& endpoint() const &
        {
            return socket_.endpoint();
        }

        auto&& endpoint(std::error_code& error) const & noexcept
        {
            return socket_.endpoint(error);
        }

        constexpr bool is_bound() const noexcept
        {
            return socket_.is_bound();
        }

        constexpr bool is_listening() const noexcept
        {
            return is_listening_;
        }

        constexpr bool is_open() const noexcept
        {
            return socket_.is_open();
        }

        void listen(int queue_size = maximum_queue_size)
        {
            std::error_code error;

            listen(error, queue_size);

            debug::throw_exception(
                error, std::source_location::current().function_name());
        }

        void listen(
            std::error_code& error,
            int              queue_size = maximum_queue_size) noexcept
        {
            if (not is_open())
            {
                error = std::make_error_code(
                    error_code_enumerator::socket_is_closed);

                return;
            }
            
            const int result = ::listen(native_handle(), queue_size);

            if (result == -1)
            {
                error = std::make_error_code(
                    error_code_enumerator {errno});
            }

            else
            {
                if (not is_bound())
                {
                    endpoint_type endpoint;

                    auto size = endpoint.size();

                    ::getsockname(native_handle(), endpoint.data(), &size);

                    socket_.endpoint_ = endpoint;
                }

                is_listening_ = true;

                error.clear();
            }
        }

        auto&& native_handle() const &
        {
            return socket_.native_handle();
        }

        auto&& native_handle(std::error_code& error) const & noexcept
        {
            return socket_.native_handle(error);
        }

        void open()
        {
            socket_.open();

            is_listening_ = false;
        }

        void open(std::error_code& error) noexcept
        {
            socket_.open(error);
        }

        static consteval protocol_enumerator protocol() noexcept
        {
            return protocol_type::protocol();
        }

        static consteval int type() noexcept
        {
            return protocol_type::type();
        }

    private:

        socket_type socket_;
        bool        is_listening_;
    };
}
