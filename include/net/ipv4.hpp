#pragma once

#include <system_error>
#include <string_view>
#include <stdexcept>
#include <optional>
#include <utility>
#include <format>
#include <string>
#include <array>

#include <cstdint>

#include <arpa/inet.h>

#include <netinet/in.h>

#include <sys/socket.h>

#include "debug/throw_exception.hpp"

#include "detail/to_network_byte_order.hpp"
#include "detail/to_host_byte_order.hpp"
#include "detail/make_error_code.hpp"

#include "generic/basic_endpoint.hpp"

#include "error_code_enumerator.hpp"
#include "protocol_enumerator.hpp"

namespace net
{
    class ipv4 final
    {
    public:

        class endpoint final : public generic::basic_endpoint
        {
        public:

            using port_type = ::in_port_t;

            constexpr endpoint() noexcept :
                address_ {
                    .sin_family = static_cast<decltype(address_.sin_family)>(
                        domain()),

                    .sin_port = 0,
                    .sin_addr = 0,
                    .sin_zero = 0
                }
            {}

            endpoint(std::string_view address, port_type port = {}) :
                endpoint {}
            {
                this->address(address);

                this->port(port);
            }

            endpoint(
                std::error_code& error,
                std::string_view address,
                port_type        port = {}) noexcept :
                    endpoint {}
            {
                if (this->address(error, address); error)
                {
                    return;
                }

                this->port(port);
            }

            std::string address() const
            {
                std::array<char, INET_ADDRSTRLEN> buffer;

                ::inet_ntop(
                    domain(),
                    &(address_.sin_addr),
                    buffer.data(),
                    buffer.size()
                );

                return buffer.data();
            }

            void address(std::string_view address)
            {
                std::error_code error;

                this->address(error, address);

                debug::throw_exception(error, __func__);
            }

            void address(
                std::error_code& error, std::string_view address) noexcept
            {
                const auto result = ::inet_pton(
                    domain(), address.data(), &(address_.sin_addr));

                if (result == 0)
                {
                    error = std::make_error_code(
                        error_code_enumerator::invalid_ipv4_address);
                }
            }

            native_handler_type* data() noexcept override
            {
                return reinterpret_cast<native_handler_type*>(&address_);
            }

            const native_handler_type* data() const noexcept override
            {
                return reinterpret_cast<const native_handler_type*>(&address_);
            }

            constexpr port_type port() const noexcept
            {
                return detail::to_host_byte_order(address_.sin_port);
            }

            constexpr void port(port_type port) noexcept
            {
                address_.sin_port = detail::to_network_byte_order(port);
            }

            constexpr size_type size() const noexcept override
            {
                return sizeof(address_);
            }

        private:

            ::sockaddr_in address_;
        };

        class header final
        {
        public:

            static constexpr std::size_t maximum_header_size = 60;
            static constexpr std::size_t minimum_header_size = 20;

            static std::optional<std::pair<header, std::string>>
            from_data(std::string_view);

            constexpr header() noexcept :
                version_and_ihl_ {0b0100'0101},
                type_of_service_ {},

                total_length_ {
                    detail::to_network_byte_order(static_cast<std::uint16_t>(
                        (version_and_ihl_ & 0b0000'1111) * 4))
                },

                identification_            {},
                flags_and_fragment_offset_ {},
                time_to_live_              {},
                protocol_                  {},
                header_checksum_           {},
                source_address_            {},
                destination_address_       {}
            {}

            constexpr bool has_options() const noexcept
            {
                return header_size() > minimum_header_size;
            }

            constexpr std::size_t header_size() const noexcept
            {
                return (version_and_ihl_ & 0b0000'1111) * 4;
            }

            header& header_size(std::size_t size);

            constexpr std::size_t packet_size() const noexcept
            {
                return detail::to_host_byte_order(total_length_);
            }

            header& packet_size(std::size_t size)
            {
                [[unlikely]] if (size < header_size())
                {
                    throw std::invalid_argument {
                        std::format(
                            "{}: "
                            "packet size must be greater than or equal to {}",
                            __func__,
                            minimum_header_size
                        )
                    };
                }

                total_length_ = detail::to_network_byte_order(
                    static_cast<std::uint16_t>(size));

                return *this;
            }

            constexpr protocol_enumerator protocol() const noexcept
            {
                return protocol_;
            }

            header& protocol(protocol_enumerator protocol) noexcept
            {
                protocol_ = protocol;

                return *this;
            }

            std::string to_string() const noexcept
            {
                std::string string(header_size(), '\0');

                *reinterpret_cast<header*>(string.data()) = *this;

                return string;
            }

            constexpr int version() const noexcept
            {
                return (version_and_ihl_ & 0b1111'0000) >> 4;
            }

        private:

            std::uint8_t        version_and_ihl_;
            std::uint8_t        type_of_service_;
            std::uint16_t       total_length_;
            std::uint16_t       identification_;
            std::uint16_t       flags_and_fragment_offset_;
            std::uint8_t        time_to_live_;
            protocol_enumerator protocol_;
            std::uint16_t       header_checksum_;
            std::uint32_t       source_address_;
            std::uint32_t       destination_address_;
        };

        inline static const endpoint loopback {"127.0.0.1", 0};

        ipv4() = delete;

        static constexpr int domain() noexcept
        {
            return AF_INET;
        }
    };
}
