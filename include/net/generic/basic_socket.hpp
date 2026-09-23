#pragma once

#include <source_location>
#include <system_error>
#include <string_view>
#include <type_traits>
#include <optional>
#include <utility>
#include <string>

#include <cerrno>

#include <sys/socket.h>

#include <unistd.h>

#include "net/debug/debug.hpp"

#include "net/error/error.hpp"

#include "net/name_requirement/protocol.hpp"

#include "net/protocol_enumerator.hpp"

namespace net::generic
{
    template<name_requirement::Protocol T>
    class basic_acceptor;

    template<name_requirement::Protocol T>
    class basic_socket final
    {
    public:

        friend basic_acceptor<T>;

        using protocol_type      = T;
        using domain_type        = protocol_type::domain_type;
        using endpoint_type      = domain_type::endpoint;
        using flags_type         = int;
        using native_handle_type = int;

        basic_socket() = default;

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
                std::swap(native_handle_,   other.native_handle_);
                std::swap(remote_endpoint_, other.remote_endpoint_);
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
            auto&& native_handle = this->native_handle(error);

            if (error)
            {
                return;
            }

            if (is_bound())
            {
                error = std::make_error_code(
                    error::code_enumerator::socket_is_already_bound);
            }

            else
            {
                const int result = ::bind(
                    native_handle.value(), endpoint.data(), endpoint.size());

                if (result == -1)
                {
                    error = std::make_error_code(
                        error::code_enumerator {errno});
                }

                else
                {
                    endpoint_type e;

                    auto s = e.size();

                    ::getsockname(native_handle.value(), e.data(), &s);

                    endpoint_ = e;

                    error.clear();
                }
            }
        }

        void connect(const endpoint_type& remote_endpoint)
        {
            std::error_code error;

            connect(error, remote_endpoint);

            debug::throw_exception(
                error, std::source_location::current().function_name());
        }

        void connect(
            std::error_code&     error,
            const endpoint_type& remote_endpoint) noexcept(
                std::is_nothrow_assignable_v<
                    std::optional<endpoint_type>, endpoint_type>)
        {
            auto&& native_handle = this->native_handle(error);

            if (error)
            {
                return;
            }

            if (is_connected())
            {
                error = std::make_error_code(
                    error::code_enumerator::socket_is_already_connected);
            }

            else
            {
                const int result = ::connect(
                    native_handle.value(),
                    remote_endpoint.data(),
                    remote_endpoint.size()
                );

                if (result == -1)
                {
                    error = std::make_error_code(
                        error::code_enumerator {errno});
                }

                else
                {
                    remote_endpoint_ = remote_endpoint;

                    if (not is_bound())
                    {
                        endpoint_type endpoint;

                        auto size = endpoint.size();

                        ::getsockname(
                            native_handle.value(), endpoint.data(), &size);

                        endpoint_ = endpoint;
                    }

                    error.clear();
                }
            }
        }

        void close() noexcept
        {
            if (is_open())
            {
                ::close(native_handle_.value());
            }

            endpoint_.reset();

            native_handle_.reset();

            remote_endpoint_.reset();
        }

        static constexpr int domain() noexcept
        {
            return domain_type::domain();
        }

        const endpoint_type& endpoint() const &
        {
            std::error_code error;

            auto&& optional = endpoint(error);

            debug::throw_exception(
                error, std::source_location::current().function_name());

            return optional.value();
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
                    error::code_enumerator::socket_is_not_bound);
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
            return native_handle_.has_value();
        }

        const native_handle_type& native_handle() const &
        {
            std::error_code error;

            auto&& optional = native_handle(error);

            debug::throw_exception(
                error, std::source_location::current().function_name());

            return optional.value();
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
                    error::code_enumerator::socket_is_closed);
            }

            return native_handle_;
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
            const auto result = ::socket(
                domain(),
                type() | SOCK_CLOEXEC,
                static_cast<int>(protocol())
            );

            if (result == -1)
            {
                error = std::make_error_code(error::code_enumerator {errno});
            }

            else
            {
                close();

                native_handle_ = result;

                error.clear();
            }
        }

        static constexpr protocol_enumerator protocol() noexcept
        {
            return protocol_type::protocol();
        }

        void receive(std::string& string, flags_type flags = {}) const
        {
            std::error_code error;

            receive(error, string, flags);

            debug::throw_exception(
                error, std::source_location::current().function_name());
        }

        void receive(
            std::error_code& error,
            std::string&     string,
            flags_type       flags = {}) const noexcept
        {
            auto&& native_handle = this->native_handle(error);

            if (error)
            {
                return;
            }

            const auto string_size_before_receiving = string.size();

            string.resize(string.capacity());

            const auto received_bytes = ::recv(
                native_handle.value(),
                string.data(),
                string.size(),
                flags
            );

            if (received_bytes == -1)
            {
                error = std::make_error_code(error::code_enumerator {errno});
            }

            else
            {
                error.clear();
            }

            string.resize(received_bytes == -1 ?
                string_size_before_receiving : received_bytes);
        }

        void receive_from(
            std::string&   string,
            endpoint_type& endpoint,
            flags_type     flags = {}) const
        {
            std::error_code error;

            receive_from(error, string, endpoint, flags);

            debug::throw_exception(
                error, std::source_location::current().function_name());
        }

        void receive_from(
            std::error_code& error,
            std::string&     string,
            endpoint_type&   endpoint,
            flags_type       flags = {}) const noexcept
        {
            auto&& native_handle = this->native_handle(error);

            if (error)
            {
                return;
            }

            const auto string_size_before_receiving = string.size();

            string.resize(string.capacity());

            auto endpoint_size = endpoint.size();

            const auto received_bytes = ::recvfrom(
                native_handle.value(),
                string.data(),
                string.size(),
                flags,
                endpoint.data(),
                &endpoint_size
            );

            if (received_bytes == -1)
            {
                error = std::make_error_code(error::code_enumerator {errno});
            }

            else
            {
                error.clear();
            }

            string.resize(received_bytes == -1 ?
                string_size_before_receiving : received_bytes);
        }

        const endpoint_type& remote_endpoint() const &
        {
            std::error_code error;

            auto&& optional = remote_endpoint(error);

            debug::throw_exception(
                error, std::source_location::current().function_name());

            return optional.value();
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
                    error::code_enumerator::socket_is_not_connected);
            }

            return remote_endpoint_;
        }

        std::size_t send(std::string_view string, flags_type flags = {}) const
        {
            std::error_code error;

            const auto sent_bytes = send(error, string, flags);

            debug::throw_exception(
                error, std::source_location::current().function_name());

            return sent_bytes;
        }

        std::size_t send(
            std::error_code& error,
            std::string_view string,
            flags_type       flags = {}) const noexcept
        {
            if (string.empty())
            {
                error.clear();

                return 0;
            }

            auto&& native_handle = this->native_handle(error);

            if (error)
            {
                return 0;
            }

            const auto sent_bytes = ::send(
                native_handle.value(), string.data(), string.size(), flags);

            if (sent_bytes == -1)
            {
                error = std::make_error_code(error::code_enumerator {errno});

                return 0;
            }

            error.clear();

            return sent_bytes;
        }

        std::size_t send_to(
            std::string_view     string,
            const endpoint_type& endpoint,
            flags_type           flags = {}) const
        {
            std::error_code error;

            const auto sent_bytes = send_to(error, string, endpoint, flags);

            debug::throw_exception(
                error, std::source_location::current().function_name());

            return sent_bytes;
        }

        std::size_t send_to(
            std::error_code&     error,
            std::string_view     string,
            const endpoint_type& endpoint,
            flags_type           flags = {}) const noexcept
        {
            if (string.empty())
            {
                error.clear();

                return 0;
            }

            auto&& native_handle = this->native_handle(error);

            if (error)
            {
                return 0;
            }

            const auto sent_bytes = ::sendto(
                native_handle.value(),
                string.data(),
                string.size(),
                flags,
                endpoint.data(),
                endpoint.size()
            );

            if (sent_bytes == -1)
            {
                error = std::make_error_code(error::code_enumerator {errno});

                return 0;
            }

            error.clear();

            return sent_bytes;
        }

        static constexpr int type() noexcept
        {
            return protocol_type::type();
        }

    private:

        std::optional<endpoint_type>      endpoint_;
        std::optional<native_handle_type> native_handle_;
        std::optional<endpoint_type>      remote_endpoint_;
    };
}
