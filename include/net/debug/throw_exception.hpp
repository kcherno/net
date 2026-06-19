#pragma once

#include <system_error>

namespace net::debug
{
    void throw_exception(
        const std::error_code& error, [[maybe_unused]] const char* where)
    {
        if (error)
        {
#ifdef NET_DEBUG_MODE__

            throw std::system_error {error, where};

#else

            throw std::system_error {error};

#endif
        }
    }
}
