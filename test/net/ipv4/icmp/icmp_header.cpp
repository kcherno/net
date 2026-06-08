#define BOOST_TEST_MODULE icmp_header

#define BOOST_TEST_DYN_LINK

#include <ostream>

#include <boost/test/unit_test.hpp>

#include "net/ipv4/icmp/header.hpp"

#include "net/ipv4/header.hpp"

#include "net/protocol_enumerator.hpp"

namespace net::ipv4::icmp
{
    std::ostream& boost_test_print_type(
        std::ostream& ostream, header::type_enumerator type)
    {
        using enum header::type_enumerator;

        switch (type)
        {
            case echo_reply:
            {
                return ostream << "net::ipv4::icmp::header::echo_reply";
            }

            case echo:
            {
                return ostream << "net::ipv4::icmp::header::echo";
            }

            default:
            {
                return ostream << "undefined net::ipv4::icmp::header::type";
            }
        }
    }
}

BOOST_AUTO_TEST_SUITE(icmp);

BOOST_AUTO_TEST_SUITE(header);

BOOST_AUTO_TEST_SUITE(from_data);

BOOST_AUTO_TEST_CASE(empty)
{
    const auto icmp_packet = net::ipv4::icmp::header::from_data({});

    BOOST_TEST(not icmp_packet.has_value());
}

BOOST_AUTO_TEST_CASE(ip_header_only)
{
    net::ipv4::header ip_header;

    ip_header.protocol(net::protocol_enumerator::icmp);

    const auto icmp_packet = net::ipv4::icmp::header::from_data(
        ip_header.to_string()
    );

    BOOST_TEST(not icmp_packet.has_value());
}

BOOST_AUTO_TEST_CASE(ip_header_with_data)
{
    net::ipv4::header ip_header;

    ip_header.protocol(net::protocol_enumerator::icmp);

    const net::ipv4::icmp::header icmp_header {
        .type     = net::ipv4::icmp::header::type_enumerator::echo,
        .code     = {},
        .checksum = {},

        .echo_message {
            .identifier      = 1,
            .sequence_number = 2
        }
    };

    const auto icmp_packet = net::ipv4::icmp::header::from_data(
        ip_header.to_string() + net::ipv4::icmp::make_icmp_message(
            icmp_header, {}));

    BOOST_REQUIRE(icmp_packet.has_value());

    const auto& [header, data] = icmp_packet.value();

    BOOST_CHECK_EQUAL(
        header.type, net::ipv4::icmp::header::type_enumerator::echo);

    BOOST_CHECK_EQUAL(header.code, 0);
    BOOST_CHECK_EQUAL(header.echo_message.identifier, 1);
    BOOST_CHECK_EQUAL(header.echo_message.sequence_number, 2);

    BOOST_TEST(data.empty());
}

BOOST_AUTO_TEST_SUITE_END(); // icmp/header/from_data

BOOST_AUTO_TEST_SUITE_END(); // icmp/header

BOOST_AUTO_TEST_CASE(make_icmp_echo_message)
{
    constexpr net::ipv4::icmp::header header {
        .type     = net::ipv4::icmp::header::type_enumerator::echo,
        .code     = 0,
        .checksum = 0,

        .echo_message {
            .identifier      = 1,
            .sequence_number = 2
        }
    };

    const auto icmp_echo_message =
        net::ipv4::icmp::make_icmp_message(header, {});

    BOOST_CHECK_EQUAL(
        icmp_echo_message.size(),
        net::ipv4::icmp::header::echo_message_header_size);

    auto icmp_echo_message_pointer =
        reinterpret_cast<const net::ipv4::icmp::header*>(
            icmp_echo_message.data());

    BOOST_CHECK_EQUAL(icmp_echo_message_pointer->type, header.type);
    BOOST_CHECK_EQUAL(icmp_echo_message_pointer->code, header.code);

    BOOST_CHECK_EQUAL(
        icmp_echo_message_pointer->echo_message.identifier,
        net::to_network_byte_order(header.echo_message.identifier));

    BOOST_CHECK_EQUAL(
        icmp_echo_message_pointer->echo_message.sequence_number,
        net::to_network_byte_order(header.echo_message.sequence_number));
}

BOOST_AUTO_TEST_CASE(make_icmp_echo_reply_message)
{
    constexpr net::ipv4::icmp::header header {
        .type     = net::ipv4::icmp::header::type_enumerator::echo_reply,
        .code     = 0,
        .checksum = 0,

        .echo_message {
            .identifier      = 1,
            .sequence_number = 2
        }
    };

    const auto icmp_echo_message =
        net::ipv4::icmp::make_icmp_message(header, {});

    BOOST_CHECK_EQUAL(
        icmp_echo_message.size(),
        net::ipv4::icmp::header::echo_message_header_size);

    auto icmp_echo_message_pointer =
        reinterpret_cast<const net::ipv4::icmp::header*>(
            icmp_echo_message.data());

    BOOST_CHECK_EQUAL(icmp_echo_message_pointer->type, header.type);
    BOOST_CHECK_EQUAL(icmp_echo_message_pointer->code, header.code);

    BOOST_CHECK_EQUAL(
        icmp_echo_message_pointer->echo_message.identifier,
        net::to_network_byte_order(header.echo_message.identifier));

    BOOST_CHECK_EQUAL(
        icmp_echo_message_pointer->echo_message.sequence_number,
        net::to_network_byte_order(header.echo_message.sequence_number));
}

BOOST_AUTO_TEST_SUITE_END(); // icmp
