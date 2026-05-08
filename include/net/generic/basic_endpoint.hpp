#pragma once

#include <sys/socket.h>

namespace net::generic
{
    template<typename T>
    class basic_endpoint
    {
    public:

        using native_handle_type = ::sockaddr;
        using size_type          = ::socklen_t;

        auto data(this auto&& self) noexcept
        {
            return self.data();
        }

        consteval auto size(this auto&& self) noexcept
        {
            return self.size();
        }

    protected:

        basic_endpoint() = default;
    };
}
