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

BOOST_AUTO_TEST_SUITE(acceptor);

BOOST_AUTO_TEST_SUITE(constructor);

BOOST_AUTO_TEST_CASE(default_constructor)
{
    net::tcp::acceptor acceptor;

    BOOST_CHECK_EXCEPTION(
        acceptor.accept(),
        std::system_error,
        net::test::accept_through_closed_socket
    );

    {
        std::error_code error;

        BOOST_CHECK_NO_THROW(acceptor.accept(error));

        BOOST_TEST(error);
    }

    BOOST_CHECK_EXCEPTION(
        acceptor.bind(net::ipv4::loopback),
        std::system_error,
        net::test::bind_through_closed_socket
    );

    {
        std::error_code error;

        BOOST_CHECK_NO_THROW(acceptor.bind(error, net::ipv4::loopback));

        BOOST_TEST(error);
    }

    BOOST_REQUIRE_NO_THROW(acceptor.close());

    BOOST_CHECK_EQUAL(acceptor.domain(), net::ipv4::domain());

    BOOST_CHECK_EXCEPTION(
        acceptor.endpoint(),
        std::system_error,
        net::test::get_endpoint_through_unbound_socket
    );

    {
        std::error_code error;

        BOOST_CHECK_NO_THROW(acceptor.endpoint(error));

        BOOST_TEST(error);
    }

    BOOST_TEST(not acceptor.is_bound());

    BOOST_TEST(not acceptor.is_listening());

    BOOST_TEST(not acceptor.is_open());

    BOOST_CHECK_EXCEPTION(
        acceptor.listen(),
        std::system_error,
        net::test::listen_through_closed_socket
    );

    {
        std::error_code error;

        BOOST_CHECK_NO_THROW(acceptor.listen(error));

        BOOST_TEST(error);
    }

    BOOST_CHECK_EXCEPTION(
        acceptor.native_handle(),
        std::system_error,
        net::test::native_handle_through_closed_socket
    );

    {
        std::error_code error;

        BOOST_CHECK_NO_THROW(acceptor.native_handle(error));

        BOOST_TEST(error);
    }

    BOOST_REQUIRE_NO_THROW(acceptor.open());

    {
        std::error_code error;

        BOOST_CHECK_NO_THROW(acceptor.open(error));

        BOOST_TEST(not error);
    }

    BOOST_CHECK_EQUAL(acceptor.protocol(), net::tcp::protocol());

    BOOST_CHECK_EQUAL(acceptor.type(), net::tcp::type());
}

BOOST_AUTO_TEST_SUITE(move_constructor);

BOOST_AUTO_TEST_CASE(move_closed_acceptor)
{
    net::tcp::acceptor acceptor_1;

    BOOST_TEST(not acceptor_1.is_open());

    const net::tcp::acceptor acceptor_2 {std::move(acceptor_1)};

    BOOST_TEST(not acceptor_1.is_open());

    BOOST_TEST(not acceptor_2.is_open());
}

BOOST_AUTO_TEST_CASE(move_open_acceptor)
{
    net::tcp::acceptor acceptor_1;

    BOOST_TEST(not acceptor_1.is_open());

    BOOST_REQUIRE_NO_THROW(acceptor_1.open());

    BOOST_TEST(acceptor_1.is_open());

    const net::tcp::acceptor acceptor_2 {std::move(acceptor_1)};

    BOOST_TEST(not acceptor_1.is_open());

    BOOST_TEST(acceptor_2.is_open());
}

BOOST_AUTO_TEST_CASE(move_bound_acceptor)
{
    net::tcp::acceptor acceptor_1;

    BOOST_TEST(not acceptor_1.is_bound());

    BOOST_TEST(not acceptor_1.is_open());

    BOOST_REQUIRE_NO_THROW(acceptor_1.open());

    BOOST_REQUIRE_NO_THROW(acceptor_1.bind(net::ipv4::loopback));

    BOOST_TEST(acceptor_1.is_bound());
    BOOST_TEST(acceptor_1.is_open());

    const net::tcp::acceptor acceptor_2 {std::move(acceptor_1)};

    BOOST_TEST(not acceptor_1.is_bound());

    BOOST_TEST(not acceptor_1.is_open());

    BOOST_TEST(acceptor_2.is_bound());

    BOOST_TEST(acceptor_2.is_open());
}

BOOST_AUTO_TEST_CASE(move_listening_acceptor)
{
    net::tcp::acceptor acceptor_1;

    BOOST_TEST(not acceptor_1.is_bound());

    BOOST_TEST(not acceptor_1.is_listening());

    BOOST_TEST(not acceptor_1.is_open());

    BOOST_REQUIRE_NO_THROW(acceptor_1.open());

    BOOST_REQUIRE_NO_THROW(acceptor_1.bind(net::ipv4::loopback));

    BOOST_REQUIRE_NO_THROW(acceptor_1.listen());

    BOOST_TEST(acceptor_1.is_bound());

    BOOST_TEST(acceptor_1.is_listening());

    BOOST_TEST(acceptor_1.is_open());

    const net::tcp::acceptor acceptor_2 {std::move(acceptor_1)};

    BOOST_TEST(not acceptor_1.is_bound());

    BOOST_TEST(not acceptor_1.is_listening());

    BOOST_TEST(not acceptor_1.is_open());

    BOOST_TEST(acceptor_2.is_bound());

    BOOST_TEST(acceptor_2.is_listening());

    BOOST_TEST(acceptor_2.is_open());
}

BOOST_AUTO_TEST_SUITE_END(); // tcp/acceptor/move_constructor

BOOST_AUTO_TEST_CASE(parameterized_constructor)
{
    net::tcp::acceptor acceptor {net::ipv4::loopback};

    BOOST_CHECK_EXCEPTION(
        acceptor.bind(net::ipv4::loopback),
        std::system_error,
        net::test::bind_through_already_bound_socket
    );

    {
        std::error_code error;

        BOOST_CHECK_NO_THROW(acceptor.bind(error, net::ipv4::loopback));

        BOOST_TEST(error);
    }

    BOOST_REQUIRE_NO_THROW(acceptor.endpoint());

    {
        std::error_code error;

        BOOST_CHECK_NO_THROW(acceptor.endpoint(error));

        BOOST_TEST(not error);
    }

    BOOST_TEST(acceptor.is_bound());

    BOOST_TEST(acceptor.is_listening());

    BOOST_TEST(acceptor.is_open());

    BOOST_REQUIRE_NO_THROW(acceptor.listen());

    {
        std::error_code error;

        BOOST_CHECK_NO_THROW(acceptor.listen(error));

        BOOST_TEST(not error);
    }

    BOOST_REQUIRE_NO_THROW(acceptor.native_handle());

    {
        std::error_code error;

        BOOST_CHECK_NO_THROW(acceptor.native_handle(error));

        BOOST_TEST(not error);
    }

    BOOST_REQUIRE_NO_THROW(acceptor.open());

    {
        std::error_code error;

        BOOST_CHECK_NO_THROW(acceptor.open(error));

        BOOST_TEST(not error);
    }
}

BOOST_AUTO_TEST_SUITE_END(); // tcp/acceptor/constructor

BOOST_AUTO_TEST_SUITE(assignment_operator);

BOOST_AUTO_TEST_SUITE(move_assignment);

BOOST_AUTO_TEST_CASE(move_closed_acceptor)
{
    net::tcp::acceptor acceptor_1;

    BOOST_TEST(not acceptor_1.is_open());

    net::tcp::acceptor acceptor_2;

    BOOST_TEST(not acceptor_2.is_open());

    acceptor_2 = std::move(acceptor_1);

    BOOST_TEST(not acceptor_1.is_open());

    BOOST_TEST(not acceptor_2.is_open());
}

BOOST_AUTO_TEST_CASE(move_open_acceptor)
{
    net::tcp::acceptor acceptor_1;

    BOOST_TEST(not acceptor_1.is_open());

    BOOST_REQUIRE_NO_THROW(acceptor_1.open());

    BOOST_TEST(acceptor_1.is_open());

    net::tcp::acceptor acceptor_2;

    BOOST_TEST(not acceptor_2.is_open());

    acceptor_2 = std::move(acceptor_1);

    BOOST_TEST(not acceptor_1.is_open());

    BOOST_TEST(acceptor_2.is_open());
}

BOOST_AUTO_TEST_CASE(move_bound_acceptor)
{
    net::tcp::acceptor acceptor_1;

    BOOST_TEST(not acceptor_1.is_open());

    BOOST_TEST(not acceptor_1.is_bound());

    BOOST_REQUIRE_NO_THROW(acceptor_1.open());

    BOOST_REQUIRE_NO_THROW(acceptor_1.bind(net::ipv4::loopback));

    BOOST_TEST(acceptor_1.is_open());

    BOOST_TEST(acceptor_1.is_bound());

    net::tcp::acceptor acceptor_2;

    BOOST_TEST(not acceptor_2.is_open());

    BOOST_TEST(not acceptor_2.is_bound());

    acceptor_2 = std::move(acceptor_1);

    BOOST_TEST(not acceptor_1.is_open());

    BOOST_TEST(not acceptor_1.is_bound());

    BOOST_TEST(acceptor_2.is_open());

    BOOST_TEST(acceptor_2.is_bound());
}

BOOST_AUTO_TEST_CASE(move_listening_acceptor)
{
    net::tcp::acceptor acceptor_1;

    BOOST_TEST(not acceptor_1.is_open());

    BOOST_TEST(not acceptor_1.is_bound());

    BOOST_TEST(not acceptor_1.is_listening());

    BOOST_REQUIRE_NO_THROW(acceptor_1.open());

    BOOST_REQUIRE_NO_THROW(acceptor_1.bind(net::ipv4::loopback));

    BOOST_REQUIRE_NO_THROW(acceptor_1.listen());

    BOOST_TEST(acceptor_1.is_open());

    BOOST_TEST(acceptor_1.is_bound());

    BOOST_TEST(acceptor_1.is_listening());

    net::tcp::acceptor acceptor_2;

    BOOST_TEST(not acceptor_2.is_open());

    BOOST_TEST(not acceptor_2.is_bound());

    BOOST_TEST(not acceptor_2.is_listening());

    acceptor_2 = std::move(acceptor_1);

    BOOST_TEST(not acceptor_1.is_open());

    BOOST_TEST(not acceptor_1.is_bound());

    BOOST_TEST(not acceptor_1.is_listening());

    BOOST_TEST(acceptor_2.is_open());

    BOOST_TEST(acceptor_2.is_bound());

    BOOST_TEST(acceptor_2.is_listening());
}

BOOST_AUTO_TEST_SUITE_END();

// tcp/acceptor/assignment_operator/move_assignment

BOOST_AUTO_TEST_SUITE_END(); // tcp/acceptor/assignment_operator

BOOST_AUTO_TEST_SUITE(accept);

BOOST_AUTO_TEST_CASE(accept_through_closed_socket)
{
    net::tcp::acceptor acceptor;

    BOOST_CHECK_EXCEPTION(
        acceptor.accept(),
        std::system_error,
        net::test::accept_through_closed_socket
    );

    std::error_code error;

    BOOST_CHECK_NO_THROW(acceptor.accept(error));

    BOOST_TEST(error);
}

BOOST_AUTO_TEST_CASE(accept_through_unbound_socket)
{
    net::tcp::acceptor acceptor;

    BOOST_REQUIRE_NO_THROW(acceptor.open());

    BOOST_CHECK_EXCEPTION(
        acceptor.accept(),
        std::system_error,
        net::test::accept_through_unbound_socket
    );

    std::error_code error;

    BOOST_CHECK_NO_THROW(acceptor.accept(error));

    BOOST_TEST(error);
}

BOOST_AUTO_TEST_CASE(accept_through_non_listening_socket)
{
    net::tcp::acceptor acceptor;

    BOOST_REQUIRE_NO_THROW(acceptor.open());

    BOOST_REQUIRE_NO_THROW(acceptor.bind(net::ipv4::loopback));

    BOOST_CHECK_EXCEPTION(
        acceptor.accept(),
        std::system_error,
        net::test::accept_through_non_listening_socket
    );

    std::error_code error;

    BOOST_CHECK_NO_THROW(acceptor.accept(error));

    BOOST_TEST(error);
}

BOOST_AUTO_TEST_CASE(successful_accepting)
{
    net::tcp::acceptor acceptor;

    BOOST_REQUIRE_NO_THROW(acceptor.open());

    BOOST_REQUIRE_NO_THROW(acceptor.bind(net::ipv4::loopback));

    BOOST_REQUIRE_NO_THROW(acceptor.listen());

    const net::tcp::socket socket_1 {acceptor.endpoint()};

    BOOST_REQUIRE_NO_THROW(acceptor.accept());

    const net::tcp::socket socket_2 {acceptor.endpoint()};

    std::error_code error;

    BOOST_CHECK_NO_THROW(acceptor.accept(error));

    BOOST_TEST(not error);
}

BOOST_AUTO_TEST_SUITE_END(); // tcp/acceptor/accept

BOOST_AUTO_TEST_SUITE(bind);

BOOST_AUTO_TEST_CASE(bind_through_closed_socket)
{
    net::tcp::acceptor acceptor;

    BOOST_CHECK_EXCEPTION(
        acceptor.bind(net::ipv4::loopback),
        std::system_error,
        net::test::bind_through_closed_socket
    );

    std::error_code error;

    BOOST_CHECK_NO_THROW(acceptor.bind(error, net::ipv4::loopback));

    BOOST_TEST(error);
}

BOOST_AUTO_TEST_CASE(bind_through_already_bound_socket)
{
    net::tcp::acceptor acceptor;

    BOOST_REQUIRE_NO_THROW(acceptor.open());

    BOOST_REQUIRE_NO_THROW(acceptor.bind(net::ipv4::loopback));

    BOOST_TEST(acceptor.is_bound());

    BOOST_CHECK_EXCEPTION(
        acceptor.bind(net::ipv4::loopback),
        std::system_error,
        net::test::bind_through_already_bound_socket
    );

    std::error_code error;

    BOOST_CHECK_NO_THROW(acceptor.bind(error, net::ipv4::loopback));

    BOOST_TEST(error);
}

BOOST_AUTO_TEST_CASE(successful_binding)
{
    net::tcp::acceptor acceptor;

    BOOST_REQUIRE_NO_THROW(acceptor.open());

    BOOST_REQUIRE_NO_THROW(acceptor.bind(net::ipv4::loopback));

    BOOST_TEST(acceptor.is_bound());
}

BOOST_AUTO_TEST_SUITE_END(); // tcp/acceptor/bind

BOOST_AUTO_TEST_SUITE(close);

BOOST_AUTO_TEST_CASE(close_already_closed_socket)
{
    net::tcp::acceptor acceptor;

    BOOST_TEST(not acceptor.is_open());

    BOOST_REQUIRE_NO_THROW(acceptor.close());
}

BOOST_AUTO_TEST_CASE(close_open_socket)
{
    net::tcp::acceptor acceptor;

    BOOST_TEST(not acceptor.is_open());

    BOOST_REQUIRE_NO_THROW(acceptor.open());

    BOOST_TEST(acceptor.is_open());

    BOOST_REQUIRE_NO_THROW(acceptor.close());

    BOOST_TEST(not acceptor.is_open());
}

BOOST_AUTO_TEST_CASE(close_bound_socket)
{
    net::tcp::acceptor acceptor;

    BOOST_TEST(not acceptor.is_open());

    BOOST_TEST(not acceptor.is_bound());

    BOOST_REQUIRE_NO_THROW(acceptor.open());

    BOOST_REQUIRE_NO_THROW(acceptor.bind(net::ipv4::loopback));

    BOOST_TEST(acceptor.is_open());

    BOOST_TEST(acceptor.is_bound());

    BOOST_REQUIRE_NO_THROW(acceptor.close());

    BOOST_TEST(not acceptor.is_open());

    BOOST_TEST(not acceptor.is_bound());
}

BOOST_AUTO_TEST_CASE(close_listening_socket)
{
    net::tcp::acceptor acceptor;

    BOOST_TEST(not acceptor.is_open());

    BOOST_TEST(not acceptor.is_bound());

    BOOST_TEST(not acceptor.is_listening());

    BOOST_REQUIRE_NO_THROW(acceptor.open());

    BOOST_REQUIRE_NO_THROW(acceptor.bind(net::ipv4::loopback));

    BOOST_REQUIRE_NO_THROW(acceptor.listen());

    BOOST_TEST(acceptor.is_open());

    BOOST_TEST(acceptor.is_bound());

    BOOST_TEST(acceptor.is_listening());

    BOOST_REQUIRE_NO_THROW(acceptor.close());

    BOOST_TEST(not acceptor.is_open());

    BOOST_TEST(not acceptor.is_bound());

    BOOST_TEST(not acceptor.is_listening());
}

BOOST_AUTO_TEST_SUITE_END(); // tcp/acceptor/close

BOOST_AUTO_TEST_CASE(domain)
{
    BOOST_TEST((std::same_as<net::tcp::acceptor::domain_type, net::ipv4>));

    BOOST_CHECK_EQUAL(net::tcp::acceptor::domain(), net::ipv4::domain());
}

BOOST_AUTO_TEST_SUITE(endpoint);

BOOST_AUTO_TEST_CASE(get_endpoint_through_closed_socket)
{
    const net::tcp::acceptor acceptor;

    BOOST_CHECK_EXCEPTION(
        acceptor.endpoint(),
        std::system_error,
        net::test::get_endpoint_through_unbound_socket
    );

    std::error_code error;

    BOOST_CHECK_NO_THROW(acceptor.endpoint(error));

    BOOST_TEST(error);
}

BOOST_AUTO_TEST_CASE(get_endpoint_through_unbound_socket)
{
    net::tcp::acceptor acceptor;

    BOOST_REQUIRE_NO_THROW(acceptor.open());

    BOOST_CHECK_EXCEPTION(
        acceptor.endpoint(),
        std::system_error,
        net::test::get_endpoint_through_unbound_socket
    );

    std::error_code error;

    BOOST_CHECK_NO_THROW(acceptor.endpoint(error));

    BOOST_TEST(error);
}

BOOST_AUTO_TEST_CASE(successful_getting_endpoint)
{
    net::tcp::acceptor acceptor;

    BOOST_REQUIRE_NO_THROW(acceptor.open());

    BOOST_REQUIRE_NO_THROW(acceptor.bind(net::ipv4::loopback));

    BOOST_REQUIRE_NO_THROW(acceptor.endpoint());

    std::error_code error;

    BOOST_CHECK_NO_THROW(acceptor.endpoint(error));

    BOOST_TEST(not error);
}

BOOST_AUTO_TEST_SUITE_END(); // tcp/acceptor/endpoint

BOOST_AUTO_TEST_CASE(is_bound)
{
    net::tcp::acceptor acceptor;

    BOOST_TEST(not acceptor.is_bound());

    BOOST_REQUIRE_NO_THROW(acceptor.open());

    BOOST_REQUIRE_NO_THROW(acceptor.bind(net::ipv4::loopback));

    BOOST_TEST(acceptor.is_bound());
}

BOOST_AUTO_TEST_CASE(is_listening)
{
    net::tcp::acceptor acceptor;

    BOOST_TEST(not acceptor.is_listening());

    BOOST_REQUIRE_NO_THROW(acceptor.open());

    BOOST_REQUIRE_NO_THROW(acceptor.bind(net::ipv4::loopback));

    BOOST_REQUIRE_NO_THROW(acceptor.listen());

    BOOST_TEST(acceptor.is_listening());
}

BOOST_AUTO_TEST_CASE(is_open)
{
    net::tcp::acceptor acceptor;

    BOOST_TEST(not acceptor.is_open());

    BOOST_REQUIRE_NO_THROW(acceptor.open());

    BOOST_TEST(acceptor.is_open());
}

BOOST_AUTO_TEST_SUITE(listen);

BOOST_AUTO_TEST_CASE(listen_through_closed_socket)
{
    net::tcp::acceptor acceptor;

    BOOST_CHECK_EXCEPTION(
        acceptor.listen(),
        std::system_error,
        net::test::listen_through_closed_socket
    );

    std::error_code error;

    BOOST_CHECK_NO_THROW(acceptor.listen(error));

    BOOST_TEST(error);
}

BOOST_AUTO_TEST_CASE(listen_through_unbound_socket)
{
    net::tcp::acceptor acceptor;

    BOOST_REQUIRE_NO_THROW(acceptor.open());

    BOOST_REQUIRE_NO_THROW(acceptor.listen());

    std::error_code error;

    BOOST_CHECK_NO_THROW(acceptor.listen(error));

    BOOST_TEST(not error);

    BOOST_REQUIRE_NO_THROW(acceptor.endpoint());

    BOOST_CHECK_NO_THROW(acceptor.endpoint(error));

    BOOST_TEST(not error);
}

BOOST_AUTO_TEST_CASE(listen_through_already_listening_socket)
{
    net::tcp::acceptor acceptor;

    BOOST_REQUIRE_NO_THROW(acceptor.open());

    BOOST_REQUIRE_NO_THROW(acceptor.listen());

    BOOST_TEST(acceptor.is_listening());

    std::error_code error;

    BOOST_CHECK_NO_THROW(acceptor.listen(error)); // update queue size

    BOOST_TEST(not error);
}

BOOST_AUTO_TEST_CASE(successful_listening)
{
    net::tcp::acceptor acceptor;

    BOOST_REQUIRE_NO_THROW(acceptor.open());

    BOOST_REQUIRE_NO_THROW(acceptor.bind(net::ipv4::loopback));

    BOOST_REQUIRE_NO_THROW(acceptor.listen());

    BOOST_TEST(acceptor.is_listening());
}

BOOST_AUTO_TEST_SUITE_END(); // tcp/acceptor/listen

BOOST_AUTO_TEST_SUITE(native_handle);

BOOST_AUTO_TEST_CASE(get_native_handle_through_closed_socket)
{
    const net::tcp::acceptor acceptor;

    BOOST_CHECK_EXCEPTION(
        acceptor.native_handle(),
        std::system_error,
        net::test::native_handle_through_closed_socket
    );

    std::error_code error;

    BOOST_CHECK_NO_THROW(acceptor.native_handle(error));

    BOOST_TEST(error);
}

BOOST_AUTO_TEST_CASE(successful_getting_native_handle)
{
    net::tcp::acceptor acceptor;

    BOOST_REQUIRE_NO_THROW(acceptor.open());

    BOOST_REQUIRE_NO_THROW(acceptor.native_handle());

    std::error_code error;

    BOOST_CHECK_NO_THROW(acceptor.native_handle(error));

    BOOST_TEST(not error);
}

BOOST_AUTO_TEST_SUITE_END(); // tcp/acceptor/native_handle

BOOST_AUTO_TEST_SUITE(open);

BOOST_AUTO_TEST_CASE(open_closed_socket)
{
    net::tcp::acceptor acceptor;

    BOOST_TEST(not acceptor.is_open());

    BOOST_REQUIRE_NO_THROW(acceptor.open());

    BOOST_TEST(acceptor.is_open());
}

BOOST_AUTO_TEST_CASE(open_already_open_socket)
{
    net::tcp::acceptor acceptor;

    BOOST_TEST(not acceptor.is_open());

    BOOST_REQUIRE_NO_THROW(acceptor.open());

    BOOST_TEST(acceptor.is_open());

    BOOST_REQUIRE_NO_THROW(acceptor.open());

    BOOST_TEST(acceptor.is_open());
}

BOOST_AUTO_TEST_CASE(open_bound_socket)
{
    net::tcp::acceptor acceptor;

    BOOST_TEST(not acceptor.is_open());

    BOOST_REQUIRE_NO_THROW(acceptor.open());

    BOOST_TEST(acceptor.is_open());

    BOOST_REQUIRE_NO_THROW(acceptor.bind(net::ipv4::loopback));

    BOOST_TEST(acceptor.is_bound());

    BOOST_REQUIRE_NO_THROW(acceptor.open());

    BOOST_TEST(acceptor.is_open());

    BOOST_TEST(not acceptor.is_bound());
}

BOOST_AUTO_TEST_CASE(open_listening_socket)
{
    net::tcp::acceptor acceptor;

    BOOST_TEST(not acceptor.is_open());

    BOOST_REQUIRE_NO_THROW(acceptor.open());

    BOOST_TEST(acceptor.is_open());

    BOOST_REQUIRE_NO_THROW(acceptor.listen());

    BOOST_TEST(acceptor.is_bound());

    BOOST_TEST(acceptor.is_listening());

    BOOST_REQUIRE_NO_THROW(acceptor.open());

    BOOST_TEST(acceptor.is_open());

    BOOST_TEST(not acceptor.is_bound());

    BOOST_TEST(not acceptor.is_listening());
}

BOOST_AUTO_TEST_SUITE_END(); // tcp/acceptor/open

BOOST_AUTO_TEST_CASE(protocol)
{
    BOOST_TEST((std::same_as<net::tcp::acceptor::protocol_type, net::tcp>));

    BOOST_CHECK_EQUAL(net::tcp::acceptor::protocol(), net::tcp::protocol());
}

BOOST_AUTO_TEST_CASE(type)
{
    BOOST_CHECK_EQUAL(net::tcp::acceptor::type(), net::tcp::type());
}

BOOST_AUTO_TEST_SUITE_END(); // tcp/acceptor

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

BOOST_AUTO_TEST_SUITE(parameterized_constructor);

BOOST_AUTO_TEST_CASE(connect_to_non_listening_socket)
{
    net::tcp::acceptor acceptor;

    BOOST_REQUIRE_NO_THROW(acceptor.open());

    BOOST_REQUIRE_NO_THROW(acceptor.bind(net::ipv4::loopback));

    BOOST_CHECK_EXCEPTION(
        net::tcp::socket(acceptor.endpoint()),
        std::system_error,
        net::test::connect_to_non_listening_socket
    );

    std::error_code error;

    BOOST_CHECK_NO_THROW(net::tcp::socket(error, acceptor.endpoint()));

    BOOST_TEST(error);
}

BOOST_AUTO_TEST_CASE(successful_connection)
{
    const net::tcp::acceptor acceptor {net::ipv4::loopback};

    BOOST_REQUIRE_NO_THROW(net::tcp::socket(acceptor.endpoint()));

    std::error_code error;

    BOOST_CHECK_NO_THROW(net::tcp::socket(error, acceptor.endpoint()));

    BOOST_TEST(not error);
}

BOOST_AUTO_TEST_SUITE_END();

// tcp/socket/constructor/parameterized_constructor

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

BOOST_AUTO_TEST_SUITE(connect);

BOOST_AUTO_TEST_CASE(connect_through_closed_socket)
{
    const net::tcp::acceptor acceptor {net::ipv4::loopback};

    net::tcp::socket socket;

    BOOST_CHECK_EXCEPTION(
        socket.connect(acceptor.endpoint()),
        std::system_error,
        net::test::connect_through_closed_socket
    );

    std::error_code error;

    BOOST_CHECK_NO_THROW(socket.connect(error, acceptor.endpoint()));

    BOOST_TEST(error);
}

BOOST_AUTO_TEST_CASE(connect_to_non_listening_socket)
{
    net::tcp::acceptor acceptor;

    BOOST_REQUIRE_NO_THROW(acceptor.open());

    BOOST_REQUIRE_NO_THROW(acceptor.bind(net::ipv4::loopback));

    net::tcp::socket socket;

    BOOST_REQUIRE_NO_THROW(socket.open());

    BOOST_CHECK_EXCEPTION(
        socket.connect(acceptor.endpoint()),
        std::system_error,
        net::test::connect_to_non_listening_socket
    );

    std::error_code error;

    BOOST_CHECK_NO_THROW(socket.connect(error, acceptor.endpoint()));

    BOOST_TEST(error);
}

BOOST_AUTO_TEST_CASE(successful_connection)
{
    const net::tcp::acceptor acceptor {net::ipv4::loopback};

    net::tcp::socket socket;

    BOOST_REQUIRE_NO_THROW(socket.open());

    BOOST_REQUIRE_NO_THROW(socket.connect(acceptor.endpoint()));

    BOOST_REQUIRE_NO_THROW(socket.endpoint());

    BOOST_REQUIRE_NO_THROW(socket.remote_endpoint());
}

BOOST_AUTO_TEST_SUITE_END(); // tcp/socket/connect

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
