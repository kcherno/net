#pragma once

#include <system_error>
#include <string>

#include "error_code_enumerator.hpp"

namespace net
{
    class error_category final : public std::error_category
    {
    public:

        static const error_category& instantiation() noexcept
        {
            static error_category instance;

            return instance;
        }

        error_category(const error_category&) = delete;

        error_category(error_category&&) = delete;

        error_category& operator=(const error_category&) = delete;

        error_category& operator=(error_category&&) = delete;

        std::string message(int code) const override
        {
            using enum error_code_enumerator;

            switch (error_code_enumerator {code})
            {
                case success:              return "success";
                case invalid_ipv4_address: return "invalid ipv4 address";
                case socket_is_closed:     return "socket is closed";
            }

            return "undefined error";
        }

        const char* name() const noexcept override
        {
            return "net";
        }

    private:

        error_category() = default;
    };
}
