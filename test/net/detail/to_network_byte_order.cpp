#define BOOST_TEST_MODULE to_network_byte_order

#define BOOST_TEST_DYN_LINK

#include <limits>

#include <cstdint>

#include <arpa/inet.h>

#include <boost/test/unit_test.hpp>

#include "net/to_network_byte_order.hpp"

BOOST_AUTO_TEST_CASE(u16)
{
    constexpr auto u16_max = std::numeric_limits<std::uint16_t>::max();

    for (auto u16 = std::uint16_t {1}; u16 < u16_max; ++u16)
    {
        BOOST_CHECK_EQUAL(::htons(u16), net::to_network_byte_order(u16));
    }
}

BOOST_AUTO_TEST_CASE(u32)
{
    constexpr auto u32_max = std::numeric_limits<std::uint32_t>::max();

    for (auto u32 = std::uint32_t {1}; u32 < u32_max; ++u32)
    {
        BOOST_CHECK_EQUAL(::htonl(u32), net::to_network_byte_order(u32));
    }
}
