#pragma once

#include <string_view>
#include <string>

#include <netinet/in.h>

#include "basic_endpoint.hpp"

namespace net::generic
{
    class basic_ip_endpoint : public basic_endpoint
    {
    public:

        using port_type = ::in_port_t;

        virtual ~basic_ip_endpoint() = default;

        virtual std::string address() const = 0;

        virtual void address(std::string_view) = 0;

        virtual constexpr port_type port() const noexcept = 0;

        virtual constexpr void port(port_type) noexcept = 0;
    };
}
