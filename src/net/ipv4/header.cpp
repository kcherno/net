#include <string_view>
#include <stdexcept>
#include <optional>
#include <utility>
#include <format>
#include <string>

#include "net/ipv4/header.hpp"

#include "net/to_host_byte_order.hpp"

std::optional<std::pair<net::ipv4::header, std::string>>
net::ipv4::header::from_data(std::string_view data)
{
    if (data.size() >= minimum_header_size)
    {
        auto pointer = reinterpret_cast<const header*>(data.data());

        if (pointer->version() == 4)
        {

        }
    }

    return std::nullopt;
}

net::ipv4::header& net::ipv4::header::header_size(std::size_t size)
{
    if (size < minimum_header_size || size > maximum_header_size)
    {
        throw std::invalid_argument {
            std::format(
                "{}: size must be in the range from {} to {}",
                __func__,
                minimum_header_size,
                maximum_header_size
            )
        };
    }

    if (size % 4 != 0)
    {
        throw std::invalid_argument {
            std::format("{}: size must be a multiple of 4", __func__)
        };
    }

    const auto before = header_size();

    version_and_ihl_ = 0b0100'1111 & (size / 4);

    const auto after = header_size();

    packet_size(before < after ?
        packet_size() - before + after :
        packet_size() - after  + before);

    return *this;
}
