#define BOOST_TEST_MODULE icmp

#define BOOST_TEST_DYN_LINK

#include <system_error>
#include <string_view>
#include <exception>
#include <concepts>
#include <ostream>
#include <utility>

#include <boost/test/unit_test.hpp>

#include "net/detail/to_network_byte_order.hpp"

#include "net/generic/basic_acceptor.hpp"

#include "net/test/test.hpp"

#include "net/protocol_enumerator.hpp"
#include "net/icmp.hpp"
#include "net/ipv4.hpp"

namespace net
{
    std::ostream& boost_test_print_type(
        std::ostream& ostream, protocol_enumerator protocol)
    {
        using enum protocol_enumerator;

        switch (protocol)
        {
            case icmp: return ostream << "icmp";

            default: return ostream << "undefined net::protocol_enumerator";
        }
    }

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

BOOST_AUTO_TEST_SUITE(acceptor);

BOOST_AUTO_TEST_CASE(listen)
{
    net::generic::basic_acceptor<net::icmp> acceptor;

    BOOST_REQUIRE_NO_THROW(acceptor.open());

    BOOST_CHECK_EXCEPTION(
        acceptor.listen(),
        std::system_error,
        net::test::listen_operation_is_not_supported
    );
}

BOOST_AUTO_TEST_SUITE_END(); // icmp/acceptor

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

BOOST_AUTO_TEST_SUITE(socket);

BOOST_AUTO_TEST_SUITE(constructor);

BOOST_AUTO_TEST_CASE(default_constructor)
{
    net::icmp::socket socket;

    BOOST_CHECK_EXCEPTION(
        socket.bind(net::ipv4::loopback),
        std::system_error,
        net::test::bind_through_closed_socket
    );

    {
        std::error_code error;

        BOOST_CHECK_NO_THROW(socket.bind(error, net::ipv4::loopback));

        BOOST_TEST(error);
    }

    BOOST_CHECK_EXCEPTION(
        socket.connect(net::ipv4::loopback),
        std::system_error,
        net::test::connect_through_closed_socket
    );

    {
        std::error_code error;

        BOOST_CHECK_NO_THROW(socket.connect(error, net::ipv4::loopback));

        BOOST_TEST(error);
    }

    BOOST_REQUIRE_NO_THROW(socket.close());

    BOOST_CHECK_EQUAL(socket.domain(), net::ipv4::domain());

    BOOST_CHECK_EXCEPTION(
        socket.endpoint(),
        std::system_error,
        net::test::get_endpoint_through_unbound_socket
    );

    {
        std::error_code error;

        BOOST_CHECK_NO_THROW(socket.endpoint(error));

        BOOST_TEST(error);
    }

    BOOST_TEST(not socket.is_bound());

    BOOST_TEST(not socket.is_open());

    BOOST_CHECK_EXCEPTION(
        socket.native_handle(),
        std::system_error,
        net::test::native_handle_through_closed_socket
    );

    BOOST_CHECK_EQUAL(socket.protocol(), net::icmp::protocol());

    BOOST_CHECK_EXCEPTION(
        socket.remote_endpoint(),
        std::system_error,
        net::test::get_remote_endpoint_through_non_connected_socket
    );

    {
        std::error_code error;

        BOOST_CHECK_NO_THROW(socket.remote_endpoint(error));

        BOOST_TEST(error);
    }

    BOOST_CHECK_EQUAL(socket.type(), net::icmp::type());
}

BOOST_AUTO_TEST_SUITE(move_constructor);

BOOST_AUTO_TEST_CASE(move_closed_socket)
{
    net::icmp::socket socket_1;

    BOOST_TEST(not socket_1.is_open());

    const net::icmp::socket socket_2 {std::move(socket_1)};

    BOOST_TEST(not socket_1.is_open());

    BOOST_TEST(not socket_2.is_open());
}

BOOST_AUTO_TEST_CASE(move_open_socket)
{
    net::icmp::socket socket_1;

    BOOST_TEST(not socket_1.is_open());

    BOOST_REQUIRE_NO_THROW(socket_1.open());

    BOOST_TEST(socket_1.is_open());

    const net::icmp::socket socket_2 {std::move(socket_1)};

    BOOST_TEST(not socket_1.is_open());

    BOOST_TEST(socket_2.is_open());
}

BOOST_AUTO_TEST_CASE(move_bound_socket)
{
    net::icmp::socket socket_1;

    BOOST_TEST(not socket_1.is_bound());

    BOOST_REQUIRE_NO_THROW(socket_1.open());

    BOOST_REQUIRE_NO_THROW(socket_1.bind(net::ipv4::loopback));

    BOOST_TEST(socket_1.is_bound());

    const net::icmp::socket socket_2 {std::move(socket_1)};

    BOOST_TEST(not socket_1.is_bound());

    BOOST_TEST(socket_2.is_bound());
}

BOOST_AUTO_TEST_SUITE_END(); // icmp/socket/constructor/move_constructor

BOOST_AUTO_TEST_CASE(parameterized_constructor)
{
    BOOST_CHECK_NO_THROW(net::icmp::socket(net::ipv4::loopback));

    std::error_code error;

    BOOST_CHECK_NO_THROW(net::icmp::socket(error, net::ipv4::loopback));

    BOOST_TEST(not error);
}

BOOST_AUTO_TEST_SUITE_END(); // icmp/socket/constructor

BOOST_AUTO_TEST_SUITE(assignment_operator);

BOOST_AUTO_TEST_SUITE(move_assignment);

BOOST_AUTO_TEST_CASE(move_closed_socket)
{
    net::icmp::socket socket_1;

    BOOST_TEST(not socket_1.is_open());

    net::icmp::socket socket_2;

    BOOST_TEST(not socket_2.is_open());

    BOOST_CHECK_NO_THROW((socket_2 = std::move(socket_1)));

    BOOST_TEST(not socket_1.is_open());

    BOOST_TEST(not socket_2.is_open());
}

BOOST_AUTO_TEST_CASE(move_open_socket)
{
    net::icmp::socket socket_1;

    BOOST_TEST(not socket_1.is_open());

    BOOST_REQUIRE_NO_THROW(socket_1.open());

    BOOST_TEST(socket_1.is_open());

    net::icmp::socket socket_2;

    BOOST_TEST(not socket_2.is_open());

    BOOST_CHECK_NO_THROW(socket_2 = std::move(socket_1));

    BOOST_TEST(not socket_1.is_open());

    BOOST_TEST(socket_2.is_open());
}

BOOST_AUTO_TEST_CASE(move_bound_socket)
{
    net::icmp::socket socket_1;

    BOOST_TEST(not socket_1.is_bound());

    BOOST_REQUIRE_NO_THROW(socket_1.open());

    BOOST_REQUIRE_NO_THROW(socket_1.bind(net::ipv4::loopback));

    BOOST_TEST(socket_1.is_bound());

    net::icmp::socket socket_2;

    BOOST_TEST(not socket_2.is_bound());

    BOOST_REQUIRE_NO_THROW((socket_2 = std::move(socket_1)));

    BOOST_TEST(not socket_1.is_bound());

    BOOST_TEST(socket_2.is_bound());
}

BOOST_AUTO_TEST_SUITE_END(); // icmp/socket/assignment_operator/move_assignment

BOOST_AUTO_TEST_SUITE_END(); // icmp/socket/assignment_operator

BOOST_AUTO_TEST_CASE(bind)
{
    net::icmp::socket socket;

    BOOST_CHECK_EXCEPTION(
        socket.bind(net::ipv4::loopback),
        std::system_error,
        net::test::bind_through_closed_socket
    );

    std::error_code error;

    BOOST_CHECK_NO_THROW(socket.bind(error, net::ipv4::loopback));

    BOOST_TEST(error);

    BOOST_REQUIRE_NO_THROW(socket.open());

    BOOST_CHECK_NO_THROW(socket.bind(error, net::ipv4::loopback));

    BOOST_TEST(not error);
}

BOOST_AUTO_TEST_CASE(connect)
{
    net::icmp::socket socket_1;

    BOOST_REQUIRE_NO_THROW(socket_1.open());

    BOOST_REQUIRE_NO_THROW(socket_1.bind(net::ipv4::loopback));

    net::icmp::socket socket_2;

    BOOST_CHECK_EXCEPTION(
        socket_2.connect(socket_1.endpoint()),
        std::system_error,
        net::test::connect_through_closed_socket
    );

    std::error_code error;

    BOOST_CHECK_NO_THROW(socket_2.connect(error, socket_1.endpoint()));

    BOOST_TEST(error);

    BOOST_REQUIRE_NO_THROW(socket_2.open());

    BOOST_CHECK_NO_THROW(socket_2.connect(error, socket_1.endpoint()));

    BOOST_TEST(not error);
}

BOOST_AUTO_TEST_CASE(close)
{
    net::icmp::socket socket;

    BOOST_TEST(not socket.is_open());

    BOOST_REQUIRE_NO_THROW(socket.open());

    BOOST_TEST(socket.is_open());

    BOOST_REQUIRE_NO_THROW(socket.close());

    BOOST_TEST(not socket.is_open());

    BOOST_REQUIRE_NO_THROW(socket.close());

    BOOST_TEST(not socket.is_open());
}

BOOST_AUTO_TEST_CASE(domain)
{
    BOOST_TEST((std::same_as<net::icmp::socket::domain_type, net::ipv4>));

    BOOST_CHECK_EQUAL(net::icmp::socket::domain(), net::ipv4::domain());
}

BOOST_AUTO_TEST_CASE(endpoint)
{
    BOOST_TEST(
        (std::same_as<net::icmp::socket::endpoint_type, net::ipv4::endpoint>));

    net::icmp::socket socket;

    BOOST_CHECK_EXCEPTION(
        socket.endpoint(),
        std::system_error,
        net::test::get_endpoint_through_unbound_socket
    );

    std::error_code error;

    BOOST_CHECK_NO_THROW(socket.endpoint(error));

    BOOST_TEST(error);

    BOOST_REQUIRE_NO_THROW(socket.open());

    BOOST_REQUIRE_NO_THROW(socket.bind(net::ipv4::loopback));

    BOOST_REQUIRE_NO_THROW(socket.endpoint());

    BOOST_CHECK_NO_THROW(socket.endpoint(error));

    BOOST_TEST(not error);
}

BOOST_AUTO_TEST_CASE(is_bound)
{
    net::icmp::socket socket;

    BOOST_TEST(not socket.is_bound());

    BOOST_REQUIRE_NO_THROW(socket.open());

    BOOST_REQUIRE_NO_THROW(socket.bind(net::ipv4::loopback));

    BOOST_TEST(socket.is_bound());

    BOOST_REQUIRE_NO_THROW(socket.close());

    BOOST_TEST(not socket.is_bound());
}

BOOST_AUTO_TEST_CASE(is_open)
{
    net::icmp::socket socket;

    BOOST_TEST(not socket.is_open());

    BOOST_REQUIRE_NO_THROW(socket.open());

    BOOST_TEST(socket.is_open());

    BOOST_REQUIRE_NO_THROW(socket.close());

    BOOST_TEST(not socket.is_open());
}

BOOST_AUTO_TEST_CASE(native_handle)
{
    net::icmp::socket socket;

    BOOST_CHECK_EXCEPTION(
        socket.native_handle(),
        std::system_error,
        net::test::native_handle_through_closed_socket
    );

    std::error_code error;

    BOOST_CHECK_NO_THROW(socket.native_handle(error));

    BOOST_TEST(error);

    BOOST_REQUIRE_NO_THROW(socket.open());

    BOOST_REQUIRE_NO_THROW(socket.native_handle(error));

    BOOST_TEST(not error);
}

BOOST_AUTO_TEST_CASE(open)
{
    net::icmp::socket socket;

    BOOST_TEST(not socket.is_open());

    BOOST_REQUIRE_NO_THROW(socket.open());

    BOOST_TEST(socket.is_open());
}

BOOST_AUTO_TEST_CASE(protocol)
{
    BOOST_TEST((std::same_as<net::icmp::socket::protocol_type, net::icmp>));

    BOOST_CHECK_EQUAL(net::icmp::socket::protocol(), net::icmp::protocol());
}

BOOST_AUTO_TEST_CASE(remote_endpoint)
{
    BOOST_TEST(
        (std::same_as<net::icmp::socket::endpoint_type, net::ipv4::endpoint>));

    net::icmp::socket socket;

    BOOST_CHECK_EXCEPTION(
        socket.remote_endpoint(),
        std::system_error,
        net::test::get_remote_endpoint_through_non_connected_socket
    );

    std::error_code error;

    BOOST_CHECK_NO_THROW(socket.remote_endpoint(error));

    BOOST_TEST(error);

    BOOST_REQUIRE_NO_THROW((socket = net::icmp::socket(net::ipv4::loopback)));

    BOOST_REQUIRE_NO_THROW(socket.remote_endpoint());

    BOOST_CHECK_NO_THROW(socket.remote_endpoint(error));

    BOOST_TEST(not error);
}

BOOST_AUTO_TEST_CASE(type)
{
    BOOST_CHECK_EQUAL(net::icmp::socket::type(), net::icmp::type());
}

BOOST_AUTO_TEST_SUITE_END(); // icmp/socket

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
