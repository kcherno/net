#define BOOST_TEST_MODULE endpoint

#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include "net/name_requirement/endpoint.hpp"

#include "net/ipv4.hpp"

BOOST_AUTO_TEST_SUITE(ipv4);

BOOST_AUTO_TEST_CASE(endpoint)
{
    BOOST_TEST(net::name_requirement::Endpoint<net::ipv4::endpoint>);
}

BOOST_AUTO_TEST_SUITE_END(); // ipv4
