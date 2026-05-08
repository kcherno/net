#include <string_view>
#include <stdexcept>
#include <optional>
#include <utility>
#include <string>

#include "net/detail/to_network_byte_order.hpp"
#include "net/detail/calculate_checksum.hpp"
#include "net/detail/to_host_byte_order.hpp"

#include "net/protocol_enumerator.hpp"
#include "net/icmp.hpp"
#include "net/ipv4.hpp"

namespace
{
    std::string make_icmp_echo_message(
        const net::icmp::header& header, std::string_view data)
    {
        std::string icmp_echo_message(
            net::icmp::header::echo_message_header_size, '\0');

        icmp_echo_message += data;

        auto icmp_header = reinterpret_cast<net::icmp::header*>(
            icmp_echo_message.data());

        icmp_header->type = header.type;

        icmp_header->echo_message.identifier =
            net::detail::to_network_byte_order(header.echo_message.identifier);

        icmp_header->echo_message.sequence_number =
            net::detail::to_network_byte_order(
                header.echo_message.sequence_number);

        icmp_header->checksum = net::detail::calculate_checksum(
            icmp_echo_message);

        return icmp_echo_message;
    }

    std::optional<std::pair<net::icmp::header, std::string>>
    unpack_icmp_echo_message(std::string_view data)
    {
        auto pointer = reinterpret_cast<const net::icmp::header*>(data.data());

        return std::optional {
            std::pair {
                net::icmp::header {
                    .type     = pointer->type,
                    .code     = pointer->code,
                    .checksum = pointer->checksum,

                    .echo_message {
                        .identifier = net::detail::to_host_byte_order(
                            pointer->echo_message.identifier),

                        .sequence_number = net::detail::to_host_byte_order(
                            pointer->echo_message.sequence_number)
                    }
                },

                std::string {
                    data.substr(net::icmp::header::echo_message_header_size)
                }
            }
        };
    }
}

std::optional<std::pair<net::icmp::header, std::string>>
net::icmp::header::from_data(std::string_view data)
{
    const auto ip_packet = ipv4::header::from_data(data);

    [[likely]] if (ip_packet.has_value())
    {
        const auto& [ip_header, ip_data] = ip_packet.value();

        if (ip_header.protocol() == protocol_enumerator::icmp)
        {
            auto pointer = reinterpret_cast<const header*>(ip_data.data());

            using enum header::type_enumerator;

            switch (pointer->type)
            {
                case echo_reply:
                case echo:
                {
                    if (ip_data.size() >= header::echo_message_header_size)
                    {
                        return unpack_icmp_echo_message(ip_data);
                    }

                    break;
                }
            }
        }
    }

    return std::nullopt;
}

std::string net::icmp::make_icmp_message(
    const header& header, std::string_view data)
{
    using enum header::type_enumerator;

    [[likely]] switch (header.type)
    {
        case echo_reply:
        case echo:
        {
            return make_icmp_echo_message(header, data);
        }
    }

    throw std::invalid_argument {
        std::format("{}: undefined icmp::header::type_enumerator", __func__)
    };
}
