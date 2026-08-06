#define BOOST_TEST_MODULE protocol

#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include "net/name_requirement/protocol.hpp"

#include "net/icmp.hpp"
#include "net/tcp.hpp"

BOOST_AUTO_TEST_CASE(icmp)
{
    BOOST_TEST(net::name_requirement::Protocol<net::icmp>);
}

BOOST_AUTO_TEST_CASE(tcp)
{
    BOOST_TEST(net::name_requirement::Protocol<net::tcp>);
}
