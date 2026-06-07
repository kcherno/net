#define BOOST_TEST_MODULE ip_header

#define BOOST_TEST_DYN_LINK

#include <string_view>
#include <stdexcept>
#include <ostream>
#include <string>

#include <boost/test/unit_test.hpp>

#include "net/ipv4/header.hpp"

#include "net/to_network_byte_order.hpp"
#include "net/protocol_enumerator.hpp"
#include "net/to_host_byte_order.hpp"

namespace net
{
    std::ostream& boost_test_print_type(
        std::ostream& ostream, protocol_enumerator protocol)
    {
        using enum protocol_enumerator;

        switch (protocol)
        {
            case icmp: return ostream << "net::protocol_enumerator::icmp";
        }

        return ostream;
    }
}

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
    const auto packet = net::ipv4::header::from_data({});

    BOOST_TEST(not packet.has_value());
}

BOOST_AUTO_TEST_CASE(too_small_data)
{
    const auto packet = net::ipv4::header::from_data(std::string(
        net::ipv4::header::minimum_header_size - 1, '\0'));

    BOOST_TEST(not packet.has_value());
}

BOOST_AUTO_TEST_CASE(header_only)
{
    net::ipv4::header header;

    const auto packet = net::ipv4::header::from_data(header.to_string());

    BOOST_REQUIRE(packet.has_value());

    const auto& [ip_header, ip_data] = packet.value();

    BOOST_CHECK_EQUAL(ip_header.header_size(), header.header_size());
    BOOST_CHECK_EQUAL(ip_header.packet_size(), header.packet_size());

    BOOST_CHECK(ip_data.empty());
}

BOOST_AUTO_TEST_CASE(header_with_data)
{
    net::ipv4::header header;
    std::string       data(8, '\0');

    header.packet_size(header.header_size() + data.size())
        .protocol(net::protocol_enumerator::icmp);

    const auto packet = net::ipv4::header::from_data(
        header.to_string() + data);

    BOOST_REQUIRE(packet.has_value());

    const auto& [ip_header, ip_data] = packet.value();

    BOOST_CHECK_EQUAL(ip_header.header_size(), header.header_size());
    BOOST_CHECK_EQUAL(ip_header.packet_size(), header.packet_size());

    BOOST_CHECK_EQUAL(
        ip_header.packet_size(), header.header_size() + data.size());

    BOOST_CHECK_EQUAL(ip_header.protocol(), net::protocol_enumerator::icmp);

    BOOST_CHECK_EQUAL(ip_data.size(), data.size());
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

    BOOST_CHECK_EQUAL(header.header_size(), header.minimum_header_size);
    BOOST_CHECK_EQUAL(header.packet_size(), header.minimum_header_size);

    BOOST_CHECK_NO_THROW(header.header_size(52));

    BOOST_CHECK_EQUAL(header.header_size(), 52);
    BOOST_CHECK_EQUAL(header.packet_size(), 52);
}

BOOST_AUTO_TEST_SUITE_END(); // ipv4/header/header_size

BOOST_AUTO_TEST_SUITE(packet_size);

BOOST_AUTO_TEST_CASE(invalid_size)
{
    net::ipv4::header header;

    BOOST_CHECK_EXCEPTION(
        header.packet_size(0), std::invalid_argument, [](auto&& exception)
        {
            return std::string_view(exception.what()) ==
                "packet_size: size must be greater than header_size()";
        }
    );
}

BOOST_AUTO_TEST_CASE(valid_size)
{
    net::ipv4::header header;

    BOOST_CHECK_EQUAL(header.packet_size(), 20);

    header.packet_size(header.header_size() + 8);

    BOOST_CHECK_EQUAL(header.packet_size(), 28);

    header.packet_size(header.header_size() + 4);

    BOOST_CHECK_EQUAL(header.packet_size(), 24);

    header.header_size(60);

    BOOST_CHECK_EQUAL(header.packet_size(), 64);
}

BOOST_AUTO_TEST_SUITE_END(); // ipv4/header/packet_size

BOOST_AUTO_TEST_SUITE_END(); // ipv4/header

BOOST_AUTO_TEST_SUITE_END(); // ipv4
