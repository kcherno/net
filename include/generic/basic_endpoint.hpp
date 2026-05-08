#pragma once

#include <sys/socket.h>

namespace net::generic
{
    class basic_endpoint
    {
    public:

        using native_handler_type = ::sockaddr;
        using size_type           = ::socklen_t;

        virtual ~basic_endpoint() = default;

        virtual native_handler_type* data() noexcept = 0;

        virtual const native_handler_type* data() const noexcept = 0;

        virtual constexpr size_type size() const noexcept = 0;
    };
}
