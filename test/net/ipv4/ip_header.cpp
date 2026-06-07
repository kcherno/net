#define BOOST_TEST_MODULE ip_header

#define BOOST_TEST_DYN_LINK

#include <string_view>
#include <stdexcept>
#include <string>

#include <boost/test/unit_test.hpp>

#include "net/ipv4/header.hpp"

#include "net/to_network_byte_order.hpp"
#include "net/to_host_byte_order.hpp"

BOOST_AUTO_TEST_SUITE(ipv4);

BOOST_AUTO_TEST_SUITE(header);

BOOST_AUTO_TEST_SUITE(constructor);

BOOST_AUTO_TEST_CASE(default_constructor)
{
    const net::ipv4::header header;

    BOOST_CHECK_EQUAL(header.version(), 4);
    BOOST_CHECK_EQUAL(header.header_size(), header.minimum_header_size);

    BOOST_TEST(not header.has_options());
}

BOOST_AUTO_TEST_SUITE_END(); // ipv4/header/constructor

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
}

BOOST_AUTO_TEST_CASE(header_with_data)
{
}

BOOST_AUTO_TEST_SUITE_END(); // ipv4/header/from_data

BOOST_AUTO_TEST_SUITE(header_size);

BOOST_AUTO_TEST_CASE(invalid_size)
{
    net::ipv4::header header;

    BOOST_CHECK_EXCEPTION(
        header.header_size(0), std::invalid_argument, [](auto&& exception)
        {
            return std::string_view(exception.what()) ==
                "header_size: size must be in the range from 20 to 60";
        }
    );

    BOOST_CHECK_EXCEPTION(
        header.header_size(42), std::invalid_argument, [](auto&& exception)
        {
            return std::string_view(exception.what()) ==
                "header_size: size must be a multiple of 4";
        }
    );
}

BOOST_AUTO_TEST_CASE(valid_size)
{
    net::ipv4::header header;

    BOOST_CHECK_NO_THROW(header.header_size(52));

    BOOST_CHECK_EQUAL(header.header_size(), 52);
}

BOOST_AUTO_TEST_SUITE_END(); // ipv4/header/header_size

BOOST_AUTO_TEST_SUITE_END(); // ipv4/header

BOOST_AUTO_TEST_SUITE_END(); // ipv4
