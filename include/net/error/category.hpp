#pragma once

#include <system_error>
#include <string>

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

        std::string message(int) const override;

        const char* name() const noexcept override
        {
            return "net";
        }

    private:

        category() = default;
    };
}
