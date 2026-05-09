#include <string_view>
#include <stdexcept>
#include <string>

#include "net/ipv4/icmp/header.hpp"

#include "net/ipv4/calculate_checksum.hpp"

#include "net/to_network_byte_order.hpp"

using namespace net::ipv4::icmp;

namespace
{
    std::string make_icmp_echo_message(
        const header& header, std::string_view data)
    {
        std::string icmp_echo_message(header::echo_message_header_size, '\0');

        icmp_echo_message += data;

        auto icmp_header = reinterpret_cast<header*>(icmp_echo_message.data());

        icmp_header->type = header::type_enumerator::echo;

        icmp_header->echo_message.identifier =
            to_network_byte_order(header.echo_message.identifier);

        icmp_header->echo_message.sequence_number =
            to_network_byte_order(header.echo_message.sequence_number);

        icmp_header->checksum = calculate_checksum(icmp_echo_message);

        return icmp_echo_message;
    }

    std::string make_icmp_echo_replay_message(
        const header& header, std::string_view data)
    {
        auto icmp_echo_message = make_icmp_echo_message(header, data);

        auto icmp_header = reinterpret_cast<header*>(icmp_echo_message.data());

        icmp_header->type = header::type_enumeratore::echo_replay;

        icmp_header->checksum = 0;

        icmp_header->checksum = calculate_checksum(icmp_echo_message);

        return make_icmp_echo_message;
    }
}

std::string make_icmp_message(const header& header, std::string_view data)
{
    using enum header::type_enumerator;

    switch (header.type)
    {
        case echo_replay:
        {
            return make_icmp_echo_replay_message(header, data);
        }

        case echo:
        {
            return make_icmp_echo_message(header, data);
        }

        default:
        {
            throw std::runtime_error {
                "make_icmp_message: Undefined net::ipv4::icmp::header::type"
            };
        }
    }
}

