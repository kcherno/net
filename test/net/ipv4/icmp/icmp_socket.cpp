#define BOOST_TEST_MODULE socket

#define BOOST_TEST_DYN_LINK

#include <system_error>
#include <string_view>
#include <utility>

#include <netinet/in.h>

#include <sys/socket.h>

#include <boost/test/unit_test.hpp>

#include "net/ipv4/icmp/socket.hpp"

#include "net/ipv4/endpoint.hpp"

BOOST_AUTO_TEST_SUITE(icmp);

BOOST_AUTO_TEST_SUITE(socket);

BOOST_AUTO_TEST_SUITE(constructor);

BOOST_AUTO_TEST_CASE(default_constructor)
{
    net::ipv4::icmp::socket socket;

    BOOST_CHECK_EXCEPTION(
        socket.bind(net::ipv4::endpoint {}),
        std::system_error,
        [](const auto& exception)
        {
#ifdef NET_DEBUG_MODE__

            return std::string_view(exception.what()) ==
                "bind: socket is closed";

#else

            return std::string_view(exception.what()) == "socket is closed";

#endif
        });

    BOOST_CHECK_NO_THROW(socket.close());

    BOOST_CHECK_EQUAL(socket.domain(), AF_INET);

    net::ipv4::endpoint endpoint;

    BOOST_CHECK_EXCEPTION(
        socket.endpoint(endpoint),
        std::system_error,
        [](const auto& exception)
        {
#ifdef NET_DEBUG_MODE__

            return std::string_view(exception.what()) ==
                "endpoint: socket is closed";

#else

            return std::string_view(exception.what()) == "socket is closed";

#endif
        });

    BOOST_TEST(not socket.is_open());

    BOOST_CHECK_EXCEPTION(
        socket.native_handler(),
        std::system_error,
        [](const auto& exception)
        {
#ifdef NET_DEBUG_MODE__

            return std::string_view(exception.what()) ==
                "native_handler: socket is closed";

#else

            return std::string_view(exception.what()) == "socket is closed";

#endif
        });

    BOOST_CHECK_NO_THROW(socket.open());

    BOOST_CHECK_EQUAL(socket.protocol(), IPPROTO_ICMP);

    BOOST_CHECK_EQUAL(socket.type(), SOCK_RAW);
}

BOOST_AUTO_TEST_CASE(move_constructor)
{
    net::ipv4::icmp::socket socket_1;

    BOOST_TEST(not socket_1.is_open());

    BOOST_REQUIRE_NO_THROW(socket_1.open());

    BOOST_TEST(socket_1.is_open());

    const net::ipv4::icmp::socket socket_2 {std::move(socket_1)};

    BOOST_TEST(socket_2.is_open());

    BOOST_TEST(not socket_1.is_open());
}

BOOST_AUTO_TEST_SUITE_END(); // icmp/socket/constructor

BOOST_AUTO_TEST_SUITE(assignment_operator);

BOOST_AUTO_TEST_CASE(move_assignment)
{
    net::ipv4::icmp::socket socket_1;

    BOOST_TEST(not socket_1.is_open());

    BOOST_REQUIRE_NO_THROW(socket_1.open());

    BOOST_TEST(socket_1.is_open());

    net::ipv4::icmp::socket socket_2;

    BOOST_TEST(not socket_2.is_open());

    socket_2 = std::move(socket_1);

    BOOST_TEST(socket_2.is_open());

    BOOST_TEST(not socket_1.is_open());
}

BOOST_AUTO_TEST_SUITE_END(); // icmp/socket/assignment_operator

BOOST_AUTO_TEST_SUITE_END(); // icmp/socket

BOOST_AUTO_TEST_SUITE_END(); // icmp
