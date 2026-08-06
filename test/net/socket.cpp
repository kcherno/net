#define BOOST_TEST_MODULE socket

#define BOOST_TEST_DYN_LINK

#include <system_error>
#include <string_view>
#include <exception>
#include <ostream>
#include <utility>

#include <boost/test/unit_test.hpp>

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
}

namespace
{
    constexpr bool bind_via_closed_socket(
        const std::exception& exception) noexcept
    {
        const auto what = std::string_view {exception.what()};

#ifdef NET_DEBUG_MODE__

        return what == "bind: socket is closed";

#else

        return what == "socket is closed";

#endif
    }

    constexpr bool connect_via_closed_socket(
        const std::exception& exception) noexcept
    {
        const auto what = std::string_view {exception.what()};

#ifdef NET_DEBUG_MODE__

        return what == "connect: socket is closed";

#else

        return what == "socket is closed";

#endif
    }

    constexpr bool get_endpoint_via_closed_socket(
        const std::exception& exception)
    {
        const auto what = std::string_view {exception.what()};

#ifdef NET_DEBUG_MODE__

        return what == "endpoint: socket is closed";

#else

        return what == "socket is closed";

#endif
    }

    constexpr bool get_remote_endpoint_via_closed_socket(
        const std::exception& exception)
    {
        const auto what = std::string_view {exception.what()};

#ifdef NET_DEBUG_MODE__

        return what == "remote_endpoint: socket is closed";

#else

        return what == "socket is closed";

#endif
    }
}

BOOST_AUTO_TEST_SUITE(icmp);

BOOST_AUTO_TEST_SUITE(socket);

BOOST_AUTO_TEST_SUITE(constructor);

BOOST_AUTO_TEST_CASE(default_constructor)
{
    net::icmp::socket socket;

    BOOST_CHECK_EXCEPTION(
        socket.bind(net::icmp::endpoint {}),
        std::system_error,
        bind_via_closed_socket
    );

    {
        std::error_code error;

        BOOST_CHECK_NO_THROW(socket.bind(error, net::icmp::endpoint {}));

        BOOST_TEST(error);
    }

    BOOST_CHECK_EXCEPTION(
        socket.connect(net::icmp::endpoint {}),
        std::system_error,
        connect_via_closed_socket
    );

    {
        std::error_code error;

        BOOST_CHECK_NO_THROW(socket.connect(error, net::icmp::endpoint {}));

        BOOST_TEST(error);
    }

    BOOST_CHECK_NO_THROW(socket.close());

    BOOST_CHECK_EQUAL(socket.domain(), net::ipv4::domain());

    BOOST_CHECK_EXCEPTION(
        socket.endpoint(),
        std::system_error,
        get_endpoint_via_closed_socket
    );

    {
        std::error_code error;

        BOOST_CHECK_NO_THROW(socket.endpoint(error));

        BOOST_TEST(error);

        error.clear();

        net::icmp::endpoint endpoint;

        BOOST_CHECK_EXCEPTION(
            socket.endpoint(endpoint),
            std::system_error,
            get_endpoint_via_closed_socket
        );

        BOOST_CHECK_NO_THROW(socket.endpoint(error, endpoint));

        BOOST_TEST(error);
    }

    BOOST_TEST(not socket.is_open());

    BOOST_CHECK_EXCEPTION(
        socket.native_handler(),
        std::system_error,
        [](const auto& exception)
        {
            const auto what = std::string_view {exception.what()};

#ifdef NET_DEBUG_MODE__

            return what == "native_handler: socket is closed";

#else

            return what == "socket is closed";

#endif
        }
    );

    BOOST_CHECK_EQUAL(socket.protocol(), net::icmp::protocol());

    BOOST_CHECK_EXCEPTION(
        socket.remote_endpoint(),
        std::system_error,
        get_remote_endpoint_via_closed_socket
    );

    {
        std::error_code error;

        BOOST_CHECK_NO_THROW(socket.remote_endpoint(error));

        BOOST_TEST(error);

        error.clear();

        net::icmp::endpoint endpoint;

        BOOST_CHECK_EXCEPTION(
            socket.remote_endpoint(endpoint),
            std::system_error,
            get_remote_endpoint_via_closed_socket
        );

        BOOST_CHECK_NO_THROW(socket.remote_endpoint(error, endpoint));

        BOOST_TEST(error);
    }

    BOOST_CHECK_EQUAL(socket.type(), net::icmp::type());
}

BOOST_AUTO_TEST_CASE(move_constructor)
{
    net::icmp::socket socket_1;

    BOOST_TEST(not socket_1.is_open());

    BOOST_REQUIRE_NO_THROW(socket_1.open());

    BOOST_TEST(socket_1.is_open());

    const net::icmp::socket socket_2 {std::move(socket_1)};

    BOOST_TEST(not socket_1.is_open());

    BOOST_TEST(socket_2.is_open());
}

BOOST_AUTO_TEST_CASE(parameterized_constructor)
{
    BOOST_CHECK_NO_THROW(net::icmp::socket {net::icmp::endpoint {}});

    std::error_code error;

    BOOST_CHECK_NO_THROW(net::icmp::socket(error, net::icmp::endpoint()));

    BOOST_TEST(not error);
}

BOOST_AUTO_TEST_SUITE_END(); // icmp/socket/constructor

BOOST_AUTO_TEST_SUITE(assignment_operator);

BOOST_AUTO_TEST_CASE(move_assignment)
{
    net::icmp::socket socket_1;

    BOOST_TEST(not socket_1.is_open());

    BOOST_REQUIRE_NO_THROW(socket_1.open());

    BOOST_TEST(socket_1.is_open());

    net::icmp::socket socket_2;

    BOOST_TEST(not socket_2.is_open());

    socket_2 = std::move(socket_1);

    BOOST_TEST(not socket_1.is_open());

    BOOST_TEST(socket_2.is_open());
}

BOOST_AUTO_TEST_SUITE_END(); // icmp/socket/assignment_operator

BOOST_AUTO_TEST_CASE(bind)
{
    net::icmp::socket socket;

    BOOST_REQUIRE_NO_THROW(socket.open());

    BOOST_REQUIRE_NO_THROW(socket.bind(net::ipv4::loopback));

    BOOST_REQUIRE_NO_THROW(socket.endpoint());
}

BOOST_AUTO_TEST_CASE(connect)
{
    net::icmp::socket socket_1;

    BOOST_REQUIRE_NO_THROW(socket_1.open());

    BOOST_REQUIRE_NO_THROW(socket_1.bind(net::ipv4::loopback));

    const net::icmp::socket socket_2 {socket_1.endpoint()};

    BOOST_REQUIRE_NO_THROW(socket_2.remote_endpoint());
}

BOOST_AUTO_TEST_SUITE_END(); // icmp/socket

BOOST_AUTO_TEST_SUITE_END(); // icmp
