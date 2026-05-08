#define BOOST_TEST_MODULE domain

#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include "net/name_requirement/domain.hpp"

#include "net/ipv4.hpp"

BOOST_AUTO_TEST_SUITE(ipv4);

BOOST_AUTO_TEST_CASE(domain)
{
    BOOST_TEST(net::name_requirement::Domain<net::ipv4>);
}

BOOST_AUTO_TEST_SUITE_END(); // ipv4
