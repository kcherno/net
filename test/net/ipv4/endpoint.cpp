#define BOOST_TEST_MODULE endpoint

#define BOOST_TEST_DYN_LINK

#include <string_view>
#include <stdexcept>

#include <netinet/in.h>

#include <boost/test/unit_test.hpp>

#include "net/ipv4/endpoint.hpp"

BOOST_AUTO_TEST_SUITE(endpoint);

BOOST_AUTO_TEST_SUITE(constructor);

BOOST_AUTO_TEST_CASE(default_constructor)
{
    net::ipv4::endpoint endpoint;

    BOOST_CHECK_EQUAL(endpoint.address(), "0.0.0.0");

    BOOST_CHECK_NE(endpoint.data(), nullptr);

    BOOST_CHECK_EQUAL(endpoint.port(), 0);

    BOOST_CHECK_EQUAL(endpoint.size(), sizeof(::sockaddr_in));

    const net::ipv4::endpoint const_endpoint;

    BOOST_CHECK_EQUAL(const_endpoint.address(), "0.0.0.0");

    BOOST_CHECK_NE(const_endpoint.data(), nullptr);

    BOOST_CHECK_EQUAL(const_endpoint.port(), 0);

    BOOST_CHECK_EQUAL(const_endpoint.size(), sizeof(::sockaddr_in));
}

BOOST_AUTO_TEST_SUITE(parameterized_constructor);

BOOST_AUTO_TEST_CASE(initialization_by_invalid_address)
{
    BOOST_CHECK_EXCEPTION(
        net::ipv4::endpoint("a.b.c.d"),
        std::runtime_error,
        [](const auto& exception)
        {
            return std::string_view(exception.what()) ==
                "endpoint::address: "
                "a.b.c.d is an invalid IPv4 network address";
        }
    );
}

BOOST_AUTO_TEST_CASE(initialization_by_valid_address)
{
    BOOST_CHECK_NO_THROW(net::ipv4::endpoint {"0.0.0.0"});
}

BOOST_AUTO_TEST_SUITE_END(); // endpoint/constructor/parameterized_constructor

BOOST_AUTO_TEST_SUITE_END(); // endpoint/constructor

BOOST_AUTO_TEST_SUITE_END(); // endpoint
