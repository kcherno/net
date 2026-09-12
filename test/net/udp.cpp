#define BOOST_TEST_MODULE udp

#define BOOST_TEST_DYN_LINK

#include <system_error>
#include <exception>
#include <concepts>
#include <ostream>
#include <utility>

#include <boost/test/unit_test.hpp>

#include "net/generic/basic_acceptor.hpp"

#include "net/test/test.hpp"

#include "net/protocol_enumerator.hpp"
#include "net/ipv4.hpp"
#include "net/udp.hpp"

namespace net
{
    std::ostream& boost_test_print_type(
        std::ostream& ostream, protocol_enumerator protocol)
    {
        using enum protocol_enumerator;

        switch (protocol)
        {
            case udp:
                return ostream << "udp";

            default:
                return ostream << "undefined protocol";
        }
    }
}

BOOST_AUTO_TEST_SUITE(udp);

BOOST_AUTO_TEST_SUITE(acceptor);

BOOST_AUTO_TEST_CASE(listen)
{
    net::generic::basic_acceptor<net::udp> acceptor;

    BOOST_REQUIRE_NO_THROW(acceptor.open());

    BOOST_CHECK_EXCEPTION(
        acceptor.listen(),
        std::system_error,
        net::test::listen_operation_is_not_supported
    );
}

BOOST_AUTO_TEST_SUITE_END(); // udp/acceptor

BOOST_AUTO_TEST_SUITE(socket);

BOOST_AUTO_TEST_SUITE(constructor);

BOOST_AUTO_TEST_CASE(default_constructor)
{
    net::udp::socket socket;

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

    BOOST_CHECK_EQUAL(socket.protocol(), net::udp::protocol());

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

    BOOST_CHECK_EQUAL(socket.type(), net::udp::type());
}

BOOST_AUTO_TEST_SUITE(move_constructor);

BOOST_AUTO_TEST_CASE(move_closed_socket)
{
    net::udp::socket socket_1;

    BOOST_TEST(not socket_1.is_open());

    const net::udp::socket socket_2 {std::move(socket_1)};

    BOOST_TEST(not socket_1.is_open());

    BOOST_TEST(not socket_2.is_open());
}

BOOST_AUTO_TEST_CASE(move_open_socket)
{
    net::udp::socket socket_1;

    BOOST_TEST(not socket_1.is_open());

    BOOST_REQUIRE_NO_THROW(socket_1.open());

    BOOST_TEST(socket_1.is_open());

    const net::udp::socket socket_2 {std::move(socket_1)};

    BOOST_TEST(not socket_1.is_open());

    BOOST_TEST(socket_2.is_open());
}

BOOST_AUTO_TEST_CASE(move_bound_socket)
{
    net::udp::socket socket_1;

    BOOST_TEST(not socket_1.is_bound());

    BOOST_REQUIRE_NO_THROW(socket_1.open());

    BOOST_REQUIRE_NO_THROW(socket_1.bind(net::ipv4::loopback));

    BOOST_TEST(socket_1.is_bound());

    const net::udp::socket socket_2 {std::move(socket_1)};

    BOOST_TEST(not socket_1.is_bound());

    BOOST_TEST(socket_2.is_bound());
}

BOOST_AUTO_TEST_SUITE_END(); // udp/socket/constructor/move_constructor

BOOST_AUTO_TEST_CASE(parameterized_constructor)
{
    BOOST_CHECK_NO_THROW(net::udp::socket(net::ipv4::loopback));

    std::error_code error;

    BOOST_CHECK_NO_THROW(net::udp::socket(error, net::ipv4::loopback));

    BOOST_TEST(not error);
}

BOOST_AUTO_TEST_SUITE_END(); // udp/socket/constructor

BOOST_AUTO_TEST_SUITE(assignment_operator);

BOOST_AUTO_TEST_SUITE(move_assignment);

BOOST_AUTO_TEST_CASE(move_closed_socket)
{
    net::udp::socket socket_1;

    BOOST_TEST(not socket_1.is_open());

    net::udp::socket socket_2;

    BOOST_TEST(not socket_2.is_open());

    BOOST_CHECK_NO_THROW((socket_2 = std::move(socket_1)));

    BOOST_TEST(not socket_1.is_open());

    BOOST_TEST(not socket_2.is_open());
}

BOOST_AUTO_TEST_CASE(move_open_socket)
{
    net::udp::socket socket_1;

    BOOST_TEST(not socket_1.is_open());

    BOOST_REQUIRE_NO_THROW(socket_1.open());

    BOOST_TEST(socket_1.is_open());

    net::udp::socket socket_2;

    BOOST_TEST(not socket_2.is_open());

    BOOST_CHECK_NO_THROW(socket_2 = std::move(socket_1));

    BOOST_TEST(not socket_1.is_open());

    BOOST_TEST(socket_2.is_open());
}

BOOST_AUTO_TEST_CASE(move_bound_socket)
{
    net::udp::socket socket_1;

    BOOST_TEST(not socket_1.is_bound());

    BOOST_REQUIRE_NO_THROW(socket_1.open());

    BOOST_REQUIRE_NO_THROW(socket_1.bind(net::ipv4::loopback));

    BOOST_TEST(socket_1.is_bound());

    net::udp::socket socket_2;

    BOOST_TEST(not socket_2.is_bound());

    BOOST_REQUIRE_NO_THROW((socket_2 = std::move(socket_1)));

    BOOST_TEST(not socket_1.is_bound());

    BOOST_TEST(socket_2.is_bound());
}

BOOST_AUTO_TEST_SUITE_END(); // udp/socket/assignment_operator/move_assignment

BOOST_AUTO_TEST_SUITE_END(); // udp/socket/assignment_operator

BOOST_AUTO_TEST_CASE(bind)
{
    net::udp::socket socket;

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
    net::udp::socket socket_1;

    BOOST_REQUIRE_NO_THROW(socket_1.open());

    BOOST_REQUIRE_NO_THROW(socket_1.bind(net::ipv4::loopback));

    net::udp::socket socket_2;

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
    net::udp::socket socket;

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
    BOOST_TEST((std::same_as<net::udp::socket::domain_type, net::ipv4>));

    BOOST_CHECK_EQUAL(net::udp::socket::domain(), net::ipv4::domain());
}

BOOST_AUTO_TEST_CASE(endpoint)
{
    BOOST_TEST(
        (std::same_as<net::udp::socket::endpoint_type, net::ipv4::endpoint>));

    net::udp::socket socket;

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
    net::udp::socket socket;

    BOOST_TEST(not socket.is_bound());

    BOOST_REQUIRE_NO_THROW(socket.open());

    BOOST_REQUIRE_NO_THROW(socket.bind(net::ipv4::loopback));

    BOOST_TEST(socket.is_bound());

    BOOST_REQUIRE_NO_THROW(socket.close());

    BOOST_TEST(not socket.is_bound());
}

BOOST_AUTO_TEST_CASE(is_open)
{
    net::udp::socket socket;

    BOOST_TEST(not socket.is_open());

    BOOST_REQUIRE_NO_THROW(socket.open());

    BOOST_TEST(socket.is_open());

    BOOST_REQUIRE_NO_THROW(socket.close());

    BOOST_TEST(not socket.is_open());
}

BOOST_AUTO_TEST_CASE(native_handle)
{
    net::udp::socket socket;

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
    net::udp::socket socket;

    BOOST_TEST(not socket.is_open());

    BOOST_REQUIRE_NO_THROW(socket.open());

    BOOST_TEST(socket.is_open());
}

BOOST_AUTO_TEST_CASE(protocol)
{
    BOOST_TEST((std::same_as<net::udp::socket::protocol_type, net::udp>));

    BOOST_CHECK_EQUAL(net::udp::socket::protocol(), net::udp::protocol());
}

BOOST_AUTO_TEST_CASE(remote_endpoint)
{
    BOOST_TEST(
        (std::same_as<net::udp::socket::endpoint_type, net::ipv4::endpoint>));

    net::udp::socket socket;

    BOOST_CHECK_EXCEPTION(
        socket.remote_endpoint(),
        std::system_error,
        net::test::get_remote_endpoint_through_non_connected_socket
    );

    std::error_code error;

    BOOST_CHECK_NO_THROW(socket.remote_endpoint(error));

    BOOST_TEST(error);

    BOOST_REQUIRE_NO_THROW((socket = net::udp::socket(net::ipv4::loopback)));

    BOOST_REQUIRE_NO_THROW(socket.remote_endpoint());

    BOOST_CHECK_NO_THROW(socket.remote_endpoint(error));

    BOOST_TEST(not error);
}

BOOST_AUTO_TEST_CASE(type)
{
    BOOST_CHECK_EQUAL(net::udp::socket::type(), net::udp::type());
}

BOOST_AUTO_TEST_SUITE_END(); // udp/socket

BOOST_AUTO_TEST_SUITE_END(); // udp
