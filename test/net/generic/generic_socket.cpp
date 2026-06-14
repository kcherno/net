#define BOOST_TEST_MODULE generic_socket

#define BOOST_TEST_DYN_LINK

#include <system_error>
#include <string_view>
#include <utility>

#include <boost/test/unit_test.hpp>

#include "net/generic/basic_socket.hpp"

#include "net/ipv4/endpoint.hpp"

namespace unit_test::icmp
{
    class socket final : public net::generic::basic_socket
    {
    public:

        using basic_socket::basic_socket;

        constexpr int domain() const noexcept override
        {
            return AF_INET;
        }

        constexpr int protocol() const noexcept override
        {
            return IPPROTO_ICMP;
        }

        constexpr int type() const noexcept override
        {
            return SOCK_DGRAM;
        }
    };
}

BOOST_AUTO_TEST_SUITE(generic);

BOOST_AUTO_TEST_SUITE(icmp);

BOOST_AUTO_TEST_SUITE(socket);

BOOST_AUTO_TEST_SUITE(constructor);

BOOST_AUTO_TEST_CASE(default_constructor)
{
    const unit_test::icmp::socket socket;

    BOOST_CHECK_EXCEPTION(
        socket.connect(net::ipv4::endpoint {}),
        std::system_error,
        [](auto&& exception)
        {
            return std::string_view(exception.what()) ==
                "connect: socket is closed";
        }
    );

    BOOST_CHECK_EXCEPTION(
        socket.bind(net::ipv4::endpoint {}),
        std::system_error,
        [](auto&& exception)
        {
            return std::string_view(exception.what()) ==
                "bind: socket is closed";
        }
    );

    net::ipv4::endpoint endpoint;

    BOOST_CHECK_EXCEPTION(
        socket.endpoint(endpoint),
        std::system_error,
        [](auto&& exception)
        {
            return std::string_view(exception.what()) ==
                "endpoint: socket is closed";
        }
    );

    BOOST_TEST(not socket.is_open());

    BOOST_CHECK_EXCEPTION(
        socket.native_handler(),
        std::system_error,
        [](auto&& exception)
        {
            return std::string_view(exception.what()) ==
                "native_handler: socket is closed";
        }
    );

    BOOST_CHECK_EXCEPTION(
        socket.remote_endpoint(endpoint),
        std::system_error,
        [](auto&& exception)
        {
            return std::string_view(exception.what()) ==
                "remote_endpoint: socket is closed";
        }
    );
}

BOOST_AUTO_TEST_CASE(move_constructor)
{
    unit_test::icmp::socket socket_1;

    BOOST_CHECK_NO_THROW(socket_1.open());

    BOOST_TEST(socket_1.is_open());

    const unit_test::icmp::socket socket_2 {std::move(socket_1)};

    BOOST_TEST(socket_2.is_open());

    BOOST_TEST(not socket_1.is_open());
}

BOOST_AUTO_TEST_SUITE_END(); // generic/icmp/socket/constructor

BOOST_AUTO_TEST_SUITE(assignment_operator);

BOOST_AUTO_TEST_CASE(move_assignment)
{
    unit_test::icmp::socket socket_1;

    BOOST_TEST(not socket_1.is_open());

    BOOST_CHECK_NO_THROW(socket_1.open());

    BOOST_TEST(socket_1.is_open());

    unit_test::icmp::socket socket_2;

    BOOST_TEST(not socket_2.is_open());

    socket_2 = std::move(socket_1);

    BOOST_TEST(not socket_1.is_open());

    BOOST_TEST(socket_2.is_open());
}

BOOST_AUTO_TEST_SUITE_END(); // generic/icmp/socket/assignment_operator

BOOST_AUTO_TEST_SUITE_END(); // generic/icmp/socket

BOOST_AUTO_TEST_SUITE_END(); // generic/icmp

BOOST_AUTO_TEST_SUITE_END(); // generic
