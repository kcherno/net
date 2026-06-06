#define BOOST_TEST_MODULE ip_header

#define BOOST_TEST_DYN_LINK

#include <string>

#include <boost/test/unit_test.hpp>

#include "net/ipv4/header.hpp"

#include "net/to_network_byte_order.hpp"
#include "net/to_host_byte_order.hpp"

BOOST_AUTO_TEST_SUITE(ipv4);

BOOST_AUTO_TEST_SUITE(header);

BOOST_AUTO_TEST_SUITE(from_data);

BOOST_AUTO_TEST_CASE(empty)
{
    const auto optional = net::ipv4::header::from_data({});

    BOOST_TEST(not optional.has_value());
}

BOOST_AUTO_TEST_CASE(too_small_data)
{
    const auto optional = net::ipv4::header::from_data(std::string(
        net::ipv4::header::minimum_header_size - 1, '\0'));

    BOOST_TEST(not optional.has_value());
}

BOOST_AUTO_TEST_CASE(header_only)
{
    std::string header(net::ipv4::header::minimum_header_size, '\0');

    auto pointer = reinterpret_cast<net::ipv4::header*>(header.data());

    pointer->version_and_ihl = 0b0100'0101;

    BOOST_REQUIRE_EQUAL(pointer->version(), 4);

    pointer->total_length = net::to_network_byte_order(static_cast<decltype(
        pointer->total_length)>(header.size()));

    BOOST_REQUIRE_EQUAL(net::to_host_byte_order(
        pointer->total_length), header.size());

    const auto optional = net::ipv4::header::from_data(header);

    BOOST_REQUIRE(optional.has_value());

    const auto& [ip_header, ip_data] = optional.value();

    BOOST_CHECK_EQUAL(ip_header.header_size(), header.size());
    BOOST_CHECK_EQUAL(ip_header.total_length, header.size());

    BOOST_TEST(ip_data.empty());
}

BOOST_AUTO_TEST_CASE(header_with_data)
{
    std::string header(net::ipv4::header::minimum_header_size, '\0');
    const std::string data(64, '\0');
    
    auto pointer = reinterpret_cast<net::ipv4::header*>(header.data());

    pointer->version_and_ihl = 0b0100'0101;

    BOOST_REQUIRE_EQUAL(pointer->version(), 4);

    BOOST_REQUIRE_EQUAL(pointer->header_size(), header.size());
    
    pointer->total_length = net::to_network_byte_order(
        static_cast<decltype(pointer->total_length)>(
            header.size() + data.size()));

    BOOST_REQUIRE_EQUAL(
        net::to_host_byte_order(pointer->total_length),
        header.size() + data.size());

    const auto optional = net::ipv4::header::from_data(header + data);

    BOOST_REQUIRE(optional.has_value());

    const auto& [ip_header, ip_data] = optional.value();

    BOOST_CHECK_EQUAL(ip_header.header_size(), header.size());
    BOOST_CHECK_EQUAL(ip_header.total_length, header.size() + data.size());

    BOOST_CHECK_EQUAL(ip_data.size(), data.size());
}

BOOST_AUTO_TEST_SUITE_END(); // ipv4/header/from_data

BOOST_AUTO_TEST_SUITE_END(); // ipv4/header

BOOST_AUTO_TEST_SUITE_END(); // ipv4
