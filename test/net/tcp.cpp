#define BOOST_TEST_MODULE socket

#define BOOST_TEST_DYN_LINK

#include <system_error>
#include <string_view>
#include <exception>
#include <ostream>
#include <utility>

#include <boost/test/unit_test.hpp>

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

namespace
{
    constexpr bool
    bind_via_closed_socket(const std::exception& exception) noexcept
    {
        std::string_view what {exception.what()};

#ifdef NET_DEBUG_MODE__

        return what == "void net::generic::basic_socket<T>::bind("
            "const endpoint_type&) const [with T = net::tcp; "
            "endpoint_type = net::ipv4::endpoint]: socket is closed";

#else

        return what == "socket is closed";

#endif
    }

    constexpr bool
    connect_via_closed_socket(const std::exception& exception) noexcept
    {
        std::string_view what {exception.what()};

#ifdef NET_DEBUG_MODE__

        return what == "void net::generic::basic_socket<T>::connect("
            "const endpoint_type&) const [with T = net::tcp; "
            "endpoint_type = net::ipv4::endpoint]: socket is closed";

#else

        return what == "socket is closed";

#endif
    }

    constexpr bool
    connect_to_non_listening_socket(const std::exception& exception) noexcept
    {
        std::string_view what {exception.what()};

#ifdef NET_DEBUG_MODE__

        return what == "void net::generic::basic_socket<T>::connect("
            "const endpoint_type&) const [with T = net::tcp; "
            "endpoint_type = net::ipv4::endpoint]: "
            "no socket is listening on the target address";

#else

        return what == "no socket is listening on the target address";

#endif
    }

    constexpr bool
    get_endpoint_via_closed_socket(const std::exception& exception) noexcept
    {
        std::string_view what {exception.what()};

#ifdef NET_DEBUG_MODE__

        return what ==
            "void net::generic::basic_socket<T>::endpoint(endpoint_type&) const "
            "[with T = net::tcp; endpoint_type = net::ipv4::endpoint]: "
            "socket is closed";

#else

        return what == "socket is closed";

#endif
    }

    constexpr bool get_remote_endpoint_via_closed_socket(
        const std::exception& exception) noexcept
    {
        std::string_view what {exception.what()};

#ifdef NET_DEBUG_MODE__

        return what ==
            "void net::generic::basic_socket<T>::remote_endpoint("
            "endpoint_type&) const [with T = net::tcp; "
            "endpoint_type = net::ipv4::endpoint]: socket is closed";

#else

        return what == "socket is closed";

#endif
    }
}

BOOST_AUTO_TEST_SUITE(tcp);

BOOST_AUTO_TEST_SUITE(socket);

BOOST_AUTO_TEST_SUITE(constructor);

BOOST_AUTO_TEST_CASE(default_constructor)
{
    net::tcp::socket socket;

    BOOST_CHECK_EXCEPTION(
        socket.bind(net::tcp::endpoint {}),
        std::system_error,
        bind_via_closed_socket
    );

    {
        std::error_code error;

        BOOST_CHECK_NO_THROW(socket.bind(error, net::tcp::endpoint {}));

        BOOST_TEST(error);
    }

    BOOST_CHECK_EXCEPTION(
        socket.connect(net::tcp::endpoint {}),
        std::system_error,
        connect_via_closed_socket
    );

    {
        std::error_code error;

        BOOST_CHECK_NO_THROW(socket.connect(error, net::tcp::endpoint {}));

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

        net::tcp::endpoint endpoint;

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
            std::string_view what {exception.what()};

#ifdef NET_DEBUG_MODE__

            return what ==
                "const net::generic::basic_socket<T>::native_handler_type& "
                "net::generic::basic_socket<T>::native_handler() const "
                "[with T = net::tcp; native_handler_type = int]: "
                "socket is closed";

#else

            return what == "socket is closed";

#endif
        }
    );

    BOOST_CHECK_EQUAL(socket.protocol(), net::tcp::protocol());

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

        net::tcp::endpoint endpoint;

        BOOST_CHECK_EXCEPTION(
            socket.remote_endpoint(endpoint),
            std::system_error,
            get_remote_endpoint_via_closed_socket
        );

        BOOST_CHECK_NO_THROW(socket.remote_endpoint(error, endpoint));

        BOOST_TEST(error);
    }

    BOOST_CHECK_EQUAL(socket.type(), net::tcp::type());
}

BOOST_AUTO_TEST_CASE(move_constructor)
{
    net::tcp::socket socket_1;

    BOOST_TEST(not socket_1.is_open());

    BOOST_REQUIRE_NO_THROW(socket_1.open());

    BOOST_TEST(socket_1.is_open());

    const net::tcp::socket socket_2 {std::move(socket_1)};

    BOOST_TEST(not socket_1.is_open());

    BOOST_TEST(socket_2.is_open());
}

BOOST_AUTO_TEST_CASE(parameterized_constructor)
{
    net::tcp::socket socket;

    BOOST_REQUIRE_NO_THROW(socket.open());

    BOOST_REQUIRE_NO_THROW(socket.bind(net::ipv4::loopback));

    BOOST_CHECK_EXCEPTION(
        net::tcp::socket {socket.endpoint()},
        std::system_error,
        connect_to_non_listening_socket
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

    BOOST_REQUIRE_NO_THROW(socket.open());

    BOOST_REQUIRE_NO_THROW(socket.bind(net::ipv4::loopback));

    BOOST_REQUIRE_NO_THROW(socket.endpoint());
}

BOOST_AUTO_TEST_CASE(connect)
{
    net::tcp::socket socket;

    BOOST_REQUIRE_NO_THROW(socket.open());

    BOOST_REQUIRE_NO_THROW(socket.bind(net::ipv4::loopback));

    BOOST_CHECK_EXCEPTION(
        net::tcp::socket {socket.endpoint()},
        std::system_error,
        connect_to_non_listening_socket
    );

    std::error_code error;

    BOOST_CHECK_NO_THROW(net::tcp::socket(error, socket.endpoint()));

    BOOST_TEST(error);
}

BOOST_AUTO_TEST_SUITE_END(); // tcp/socket

BOOST_AUTO_TEST_SUITE_END(); // tcp
