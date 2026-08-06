#define BOOST_TEST_MODULE calculate_checksum

#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include "net/ipv4/calculate_checksum.hpp"

BOOST_AUTO_TEST_CASE(size_0)
{
    constexpr char string[] = {0x0};

    BOOST_CHECK_EQUAL(net::ipv4::calculate_checksum(string), 0xFFFF);
}

BOOST_AUTO_TEST_CASE(size_1)
{
    constexpr char string[] = {0x12, 0x0};

    BOOST_CHECK_EQUAL(net::ipv4::calculate_checksum(string), 0xFFED);
}

BOOST_AUTO_TEST_CASE(size_2)
{
    constexpr char string[] = {0x12, 0x34, 0x0};

    BOOST_CHECK_EQUAL(net::ipv4::calculate_checksum(string), 0xCBED);
}
