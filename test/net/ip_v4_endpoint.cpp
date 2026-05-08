#define BOOST_TEST_MODULE ip_v4_endpoint

#define BOOST_TEST_DYN_LINK

#include <string_view>
#include <stdexcept>

#include <netinet/in.h>

#include <boost/test/unit_test.hpp>

#include "net/ip_v4_endpoint.hpp"

BOOST_AUTO_TEST_SUITE(ip_v4_endpoint);

BOOST_AUTO_TEST_SUITE(constructor);

BOOST_AUTO_TEST_CASE(default_constructor)
{
    net::ip_v4_endpoint endpoint;

    BOOST_CHECK_EQUAL(endpoint.address(), "0.0.0.0");

    BOOST_CHECK_NE(endpoint.data(), nullptr);

    BOOST_CHECK_EQUAL(endpoint.port(), 0);

    BOOST_CHECK_EQUAL(endpoint.size(), sizeof(::sockaddr_in));

    const net::ip_v4_endpoint const_endpoint;

    BOOST_CHECK_EQUAL(const_endpoint.address(), "0.0.0.0");

    BOOST_CHECK_NE(const_endpoint.data(), nullptr);

    BOOST_CHECK_EQUAL(const_endpoint.port(), 0);

    BOOST_CHECK_EQUAL(const_endpoint.size(), sizeof(::sockaddr_in));
}

BOOST_AUTO_TEST_SUITE(parameterized_constructor);

BOOST_AUTO_TEST_CASE(initialization_by_invalid_address)
{
    BOOST_CHECK_EXCEPTION(
        net::ip_v4_endpoint("a.b.c.d"),
        std::runtime_error,
        [](const auto& exception)
        {
            return std::string_view(exception.what()) ==
                "ip_v4_endpoint::address: invalid network address";
        }
    );
}

BOOST_AUTO_TEST_CASE(initialization_by_valid_address)
{
    BOOST_CHECK_NO_THROW(net::ip_v4_endpoint {"0.0.0.0"});
}

BOOST_AUTO_TEST_SUITE_END();

// ip_v4_endpoint/constructor/parameterized_constructor

BOOST_AUTO_TEST_SUITE_END(); // ip_v4_endpoint/constructor

BOOST_AUTO_TEST_SUITE_END(); // ip_v4_endpoint
