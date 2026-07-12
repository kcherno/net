#define BOOST_TEST_MODULE icmp

#define BOOST_TEST_DYN_LINK

#include <ostream>

#include <boost/test/unit_test.hpp>

#include "net/protocol_enumerator.hpp"
#include "net/icmp.hpp"
#include "net/ipv4.hpp"

namespace net
{
    std::ostream& boost_test_print_type(
        std::ostream& ostream, icmp::header::type_enumerator type)
    {
        using enum icmp::header::type_enumerator;

        switch (type)
        {
            case echo_reply: return ostream << "echo_reply";
            case echo:       return ostream << "echo";

            default:
                return ostream <<
                    "undefined icmp::header::type_enumerator";
        }
    }
}

BOOST_AUTO_TEST_SUITE(icmp);

BOOST_AUTO_TEST_SUITE(header);

BOOST_AUTO_TEST_SUITE(from_data);

BOOST_AUTO_TEST_CASE(empty)
{
    const auto icmp_packet = net::icmp::header::from_data({});

    BOOST_TEST(not icmp_packet.has_value());
}

BOOST_AUTO_TEST_CASE(icmp_echo_message)
{
    const auto icmp_packet = net::icmp::header::from_data(
        net::ipv4::header()
            .protocol(net::protocol_enumerator::icmp)
            .to_string() +

        net::icmp::make_icmp_message(
            net::icmp::header {
                .type = net::icmp::header::type_enumerator::echo,
                .code = 0,

                .echo_message {
                    .identifier      = 1,
                    .sequence_number = 2
                }
            },

            {}
        )
    );

    BOOST_REQUIRE(icmp_packet.has_value());

    const auto& [icmp_header, icmp_data] = icmp_packet.value();

    BOOST_CHECK_EQUAL(
        icmp_header.type, net::icmp::header::type_enumerator::echo);

    BOOST_CHECK_EQUAL(icmp_header.code,                         0);
    BOOST_CHECK_EQUAL(icmp_header.echo_message.identifier,      1);
    BOOST_CHECK_EQUAL(icmp_header.echo_message.sequence_number, 2);

    BOOST_TEST(icmp_data.empty());
}

BOOST_AUTO_TEST_SUITE_END(); // icmp/header/from_data

BOOST_AUTO_TEST_SUITE_END(); // icmp/header

BOOST_AUTO_TEST_SUITE(make_icmp_message);

BOOST_AUTO_TEST_CASE(make_icmp_echo_message)
{
    constexpr net::icmp::header header {
        .type = net::icmp::header::type_enumerator::echo,
        .code = 0,

        .echo_message {
            .identifier      = 1,
            .sequence_number = 2
        }
    };

    const auto icmp_echo_message = net::icmp::make_icmp_message(header, {});

    BOOST_CHECK_EQUAL(
        icmp_echo_message.size(), net::icmp::header::echo_message_header_size);

    auto icmp_echo_message_pointer =
        reinterpret_cast<const net::icmp::header*>(icmp_echo_message.data());

    BOOST_CHECK_EQUAL(icmp_echo_message_pointer->type, header.type);
    BOOST_CHECK_EQUAL(icmp_echo_message_pointer->code, header.code);

    BOOST_CHECK_EQUAL(
        icmp_echo_message_pointer->echo_message.identifier,
        net::detail::to_network_byte_order(header.echo_message.identifier)
    );

    BOOST_CHECK_EQUAL(
        icmp_echo_message_pointer->echo_message.sequence_number,
        net::detail::to_network_byte_order(
            header.echo_message.sequence_number)
    );
}

BOOST_AUTO_TEST_CASE(make_icmp_echo_reply_message)
{
    constexpr net::icmp::header header {
        .type = net::icmp::header::type_enumerator::echo_reply,
        .code = 0,

        .echo_message {
            .identifier      = 1,
            .sequence_number = 2
        }
    };

    const auto icmp_echo_message = net::icmp::make_icmp_message(header, {});

    BOOST_CHECK_EQUAL(
        icmp_echo_message.size(), net::icmp::header::echo_message_header_size);

    auto icmp_echo_reply_message_pointer =
        reinterpret_cast<const net::icmp::header*>(icmp_echo_message.data());

    BOOST_CHECK_EQUAL(icmp_echo_reply_message_pointer->type, header.type);
    BOOST_CHECK_EQUAL(icmp_echo_reply_message_pointer->code, header.code);

    BOOST_CHECK_EQUAL(
        icmp_echo_reply_message_pointer->echo_message.identifier,
        net::detail::to_network_byte_order(header.echo_message.identifier));

    BOOST_CHECK_EQUAL(
        icmp_echo_reply_message_pointer->echo_message.sequence_number,
        net::detail::to_network_byte_order(
            header.echo_message.sequence_number)
    );
}

BOOST_AUTO_TEST_SUITE_END(); // icmp/make_icmp_message

BOOST_AUTO_TEST_SUITE_END(); // icmp
