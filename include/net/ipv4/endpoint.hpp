#pragma once

#include <string_view>
#include <stdexcept>
#include <string>
#include <array>

#include <arpa/inet.h>

#include <netinet/in.h>

#include <sys/socket.h>

#include "generic/basic_endpoint.hpp"

#include "net/to_network_byte_order.hpp"
#include "net/to_host_byte_order.hpp"

namespace net::ipv4
{
    class endpoint final : public generic::basic_endpoint
    {
    public:

        constexpr endpoint() noexcept :
            address_(AF_INET)
        {}

        endpoint(std::string_view address, port_type port = {}) :
            endpoint {}
        {
            this->address(address);
            this->port(port);
        }

        std::string address() const override
        {
            std::array<char, INET_ADDRSTRLEN> buffer;

            ::inet_ntop(
                AF_INET, &(address_.sin_addr), buffer.data(), buffer.size());

            return std::string {buffer.data()};
        }

        void address(std::string_view address) override
        {
            const auto result = ::inet_pton(
                AF_INET, address.data(), &(address_.sin_addr));

            if (result == 0)
            {
                throw std::runtime_error {
                    "endpoint::address: invalid network address"
                };
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

        constexpr port_type port() const noexcept override
        {
            return to_host_byte_order(address_.sin_port);
        }

        constexpr void port(port_type port) noexcept override
        {
            address_.sin_port = to_network_byte_order(port);
        }

        constexpr size_type size() const noexcept override
        {
            return sizeof(address_);
        }

    private:

        ::sockaddr_in address_;
    };
}
