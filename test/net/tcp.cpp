#define BOOST_TEST_MODULE tcp

#define BOOST_TEST_DYN_LINK

#include <system_error>
#include <string_view>
#include <exception>
#include <concepts>
#include <ostream>
#include <utility>
#include <string>

#include <csignal>

#include <boost/test/unit_test.hpp>

#include "net/test/test.hpp"

#include "net/protocol_enumerator.hpp"
#include "net/ipv4.hpp"
#include "net/tcp.hpp"

namespace net
{
    std::ostream& boost_test_print_type(
        std::ostream& ostream, protocol_enumerator protocol)
    {
        using enum protocol_enumerator;

        switch (protocol)
        {
            case tcp: return ostream << "tcp";

            default: return ostream << "undefined net::protocol_enumerator";
        }
    }
}

BOOST_AUTO_TEST_SUITE(tcp);

BOOST_AUTO_TEST_SUITE(socket);

BOOST_AUTO_TEST_SUITE(constructor);

BOOST_AUTO_TEST_CASE(default_constructor)
{
    net::tcp::socket socket;

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

    BOOST_TEST(not socket.is_connected());

    BOOST_TEST(not socket.is_open());

    BOOST_CHECK_EXCEPTION(
        socket.native_handle(),
        std::system_error,
        net::test::native_handle_through_closed_socket
    );

    BOOST_CHECK_EQUAL(socket.protocol(), net::tcp::protocol());

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

    BOOST_CHECK_EQUAL(socket.type(), net::tcp::type());
}

BOOST_AUTO_TEST_CASE(move_constructor)
{
    net::tcp::socket socket_1;

    BOOST_TEST(not socket_1.is_open());

    const net::tcp::socket socket_2 {std::move(socket_1)};

    BOOST_TEST(not socket_1.is_open());

    BOOST_TEST(not socket_2.is_open());

    BOOST_REQUIRE_NO_THROW(socket_1.open());

    BOOST_TEST(socket_1.is_open());

    const net::tcp::socket socket_3 {std::move(socket_1)};

    BOOST_TEST(not socket_1.is_open());

    BOOST_TEST(socket_3.is_open());
}

BOOST_AUTO_TEST_CASE(parameterized_constructor)
{
    net::tcp::socket socket;

    BOOST_REQUIRE_NO_THROW(socket.open());

    BOOST_REQUIRE_NO_THROW(socket.bind(net::ipv4::loopback));

    BOOST_CHECK_EXCEPTION(
        net::tcp::socket {socket.endpoint()},
        std::system_error,
        net::test::connect_to_non_listening_socket
    );

    std::error_code error;

    BOOST_CHECK_NO_THROW(net::tcp::socket(error, socket.endpoint()));

    BOOST_TEST(error);
}

BOOST_AUTO_TEST_SUITE_END(); // tcp/socket/constructor

BOOST_AUTO_TEST_SUITE(assignment_operator);

BOOST_AUTO_TEST_CASE(move_assignment)
{
    net::tcp::socket socket_1;

    BOOST_TEST(not socket_1.is_open());

    BOOST_REQUIRE_NO_THROW(socket_1.open());

    BOOST_TEST(socket_1.is_open());

    net::tcp::socket socket_2;

    BOOST_TEST(not socket_2.is_open());

    socket_2 = std::move(socket_1);

    BOOST_TEST(not socket_1.is_open());

    BOOST_TEST(socket_2.is_open());
}

BOOST_AUTO_TEST_SUITE_END(); // tcp/socket/assignment_operator

BOOST_AUTO_TEST_CASE(bind)
{
    net::tcp::socket socket;

    BOOST_CHECK_EXCEPTION(
        socket.bind(net::ipv4::loopback),
        std::system_error,
        net::test::bind_through_closed_socket
    );

    std::error_code error;

    BOOST_CHECK_NO_THROW(socket.bind(error, net::ipv4::loopback));

    BOOST_TEST(error);

    BOOST_REQUIRE_NO_THROW(socket.open());

    BOOST_REQUIRE_NO_THROW(socket.bind(net::ipv4::loopback));

    BOOST_CHECK_EXCEPTION(
        socket.bind(net::ipv4::loopback),
        std::system_error,
        net::test::bind_through_already_bound_socket
    );

    BOOST_CHECK_NO_THROW(socket.bind(error, net::ipv4::loopback));

    BOOST_TEST(error);
}

BOOST_AUTO_TEST_CASE(connect)
{
    net::tcp::socket socket;

    BOOST_CHECK_EXCEPTION(
        socket.connect(net::ipv4::loopback),
        std::system_error,
        net::test::connect_through_closed_socket
    );

    BOOST_REQUIRE_NO_THROW(socket.open());

    BOOST_REQUIRE_NO_THROW(socket.bind(net::ipv4::loopback));

    BOOST_CHECK_EXCEPTION(
        net::tcp::socket(socket.endpoint()),
        std::system_error,
        net::test::connect_to_non_listening_socket
    );

    std::error_code error;

    BOOST_CHECK_NO_THROW(net::tcp::socket(error, socket.endpoint()));

    BOOST_TEST(error);
}

BOOST_AUTO_TEST_CASE(close)
{
    net::tcp::socket socket;

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
    BOOST_TEST((std::same_as<net::tcp::socket::domain_type, net::ipv4>));

    BOOST_CHECK_EQUAL(net::tcp::socket::domain(), net::ipv4::domain());
}

BOOST_AUTO_TEST_CASE(endpoint)
{
    BOOST_TEST(
        (std::same_as<net::tcp::socket::endpoint_type, net::ipv4::endpoint>));

    net::tcp::socket socket;

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

    BOOST_CHECK_NO_THROW(socket.endpoint());

    BOOST_CHECK_NO_THROW(socket.endpoint(error));

    BOOST_TEST(not error);
}

BOOST_AUTO_TEST_CASE(is_bound)
{
    net::tcp::socket socket;

    BOOST_TEST(not socket.is_bound());

    BOOST_REQUIRE_NO_THROW(socket.open());

    BOOST_REQUIRE_NO_THROW(socket.bind(net::ipv4::loopback));

    BOOST_TEST(socket.is_bound());
}

BOOST_AUTO_TEST_CASE(is_open)
{
    net::tcp::socket socket;

    BOOST_TEST(not socket.is_open());

    BOOST_REQUIRE_NO_THROW(socket.open());

    BOOST_TEST(socket.is_open());

    std::error_code error;

    BOOST_REQUIRE_NO_THROW(socket.open(error));

    BOOST_TEST(not error);
}

BOOST_AUTO_TEST_CASE(native_handle)
{
    net::tcp::socket socket;

    BOOST_CHECK_EXCEPTION(
        socket.native_handle(),
        std::system_error,
        net::test::native_handle_through_closed_socket
    );

    std::error_code error;

    BOOST_CHECK_NO_THROW(socket.native_handle(error));

    BOOST_TEST(error);

    BOOST_REQUIRE_NO_THROW(socket.open());

    BOOST_CHECK_NO_THROW(socket.native_handle(error));

    BOOST_TEST(not error);
}

BOOST_AUTO_TEST_CASE(open)
{
    net::tcp::socket socket;

    BOOST_TEST(not socket.is_open());

    BOOST_REQUIRE_NO_THROW(socket.open());

    BOOST_TEST(socket.is_open());

    std::error_code error;

    BOOST_CHECK_NO_THROW(socket.open(error));

    BOOST_TEST(socket.is_open());

    BOOST_TEST(not error);
}

BOOST_AUTO_TEST_CASE(protocol)
{
    BOOST_TEST((std::same_as<net::tcp::socket::protocol_type, net::tcp>));

    BOOST_CHECK_EQUAL(net::tcp::socket::protocol(), net::tcp::protocol());
}

BOOST_AUTO_TEST_CASE(receive)
{
    std::string string;

    net::tcp::socket socket;

    BOOST_CHECK_EXCEPTION(
        socket.receive(string),
        std::system_error,
        net::test::receive_through_closed_socket
    );

    std::error_code error;

    BOOST_CHECK_NO_THROW(socket.receive(error, string));

    BOOST_TEST(error);

    BOOST_REQUIRE_NO_THROW(socket.open());

    BOOST_CHECK_EXCEPTION(
        socket.receive(string),
        std::system_error,
        net::test::receive_through_non_connected_socket
    );

    BOOST_CHECK_NO_THROW(socket.receive(error, string));

    BOOST_TEST(error);
}

BOOST_AUTO_TEST_CASE(remote_endpoint)
{
    net::tcp::socket socket;

    BOOST_CHECK_EXCEPTION(
        socket.remote_endpoint(),
        std::system_error,
        net::test::get_remote_endpoint_through_non_connected_socket
    );

    std::error_code error;

    BOOST_CHECK_NO_THROW(socket.remote_endpoint(error));

    BOOST_TEST(error);

    BOOST_REQUIRE_NO_THROW(socket.open());

    BOOST_CHECK_EXCEPTION(
        socket.remote_endpoint(),
        std::system_error,
        net::test::get_remote_endpoint_through_non_connected_socket
    );

    BOOST_CHECK_NO_THROW(socket.remote_endpoint(error));

    BOOST_TEST(error);
}

BOOST_AUTO_TEST_CASE(send)
{
    ::signal(SIGPIPE, SIG_IGN);

    const std::string string;

    net::tcp::socket socket;

    BOOST_CHECK_EXCEPTION(
        socket.send(string),
        std::system_error,
        net::test::send_through_closed_socket
    );

    std::error_code error;

    BOOST_CHECK_NO_THROW(socket.send(error, string));

    BOOST_TEST(error);

    BOOST_REQUIRE_NO_THROW(socket.open());

    BOOST_CHECK_EXCEPTION(
        socket.send(string),
        std::system_error,
        net::test::send_through_non_connected_socket
    );

    BOOST_CHECK_NO_THROW(socket.send(error, string));

    BOOST_TEST(error);
}

BOOST_AUTO_TEST_CASE(type)
{
    BOOST_CHECK_EQUAL(net::tcp::socket::type(), net::tcp::type());
}

BOOST_AUTO_TEST_SUITE_END(); // tcp/socket

BOOST_AUTO_TEST_SUITE_END(); // tcp
