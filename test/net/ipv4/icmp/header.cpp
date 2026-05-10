#define BOOST_TEST_MODULE header

#define BOOST_TEST_DYN_LINK

#include <ostream>

#include <boost/test/unit_test.hpp>

#include "net/ipv4/icmp/header.hpp"

#include "net/to_network_byte_order.hpp"

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

BOOST_AUTO_TEST_CASE(unpack_icmp_echo_message)
{
    const auto icmp_echo_message = net::ipv4::icmp::make_icmp_message(
        net::ipv4::icmp::header {
            .type     = net::ipv4::icmp::header::type_enumerator::echo,
            .code     = 0,
            .checksum = 0,

            .echo_message {
                .identifier      = 1,
                .sequence_number = 2
            }
        },
        
        {});

    const auto [header, data] = net::ipv4::icmp::unpack_icmp_message(
        net::ipv4::icmp::header::type_enumerator::echo, icmp_echo_message);

    BOOST_CHECK_EQUAL(
        header.type, net::ipv4::icmp::header::type_enumerator::echo);

    BOOST_CHECK_EQUAL(header.code, 0);

    BOOST_CHECK_EQUAL(header.echo_message.identifier,      1);
    BOOST_CHECK_EQUAL(header.echo_message.sequence_number, 2);

    BOOST_TEST(data.empty());
}

BOOST_AUTO_TEST_CASE(unpack_icmp_echo_reply_message)
{
    const auto icmp_echo_message = net::ipv4::icmp::make_icmp_message(
        net::ipv4::icmp::header {
            .type     = net::ipv4::icmp::header::type_enumerator::echo_reply,
            .code     = 0,
            .checksum = 0,

            .echo_message {
                .identifier      = 1,
                .sequence_number = 2
            }
        },
        
        {});

    const auto [header, data] = net::ipv4::icmp::unpack_icmp_message(
        net::ipv4::icmp::header::type_enumerator::echo, icmp_echo_message);

    BOOST_CHECK_EQUAL(
        header.type, net::ipv4::icmp::header::type_enumerator::echo_reply);

    BOOST_CHECK_EQUAL(header.code, 0);

    BOOST_CHECK_EQUAL(header.echo_message.identifier,      1);
    BOOST_CHECK_EQUAL(header.echo_message.sequence_number, 2);

    BOOST_TEST(data.empty());
}

BOOST_AUTO_TEST_SUITE_END(); // icmp/header

BOOST_AUTO_TEST_SUITE_END(); // icmp
