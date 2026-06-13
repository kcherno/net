#pragma once

#include <system_error>
#include <string>

#include "code_enumerator.hpp"

namespace net::error
{
    class category final : public std::error_category
    {
    public:

        static const category& instantiation() noexcept
        {
            static category instance;

            return instance;
        }

        category(const category&) = delete;

        category(category&&) = delete;

        category& operator=(const category&) = delete;

        category& operator=(category&&) = delete;

        std::string message(int code) const override
        {
            switch (code_enumerator {code})
            {
                case code_enumerator::socket_is_closed:
                {
                    return "socket is closed";
                }
            }

            return "undefined error";
        }

        const char* name() const noexcept override
        {
            return "net";
        }

    private:

        category() = default;
    };
}
