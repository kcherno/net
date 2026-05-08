#define BOOST_TEST_MODULE ipv4

#define BOOST_TEST_DYN_LINK

#include <system_error>
#include <ostream>

#include <cstdint>

#include <boost/test/unit_test.hpp>

#include "net/protocol_enumerator.hpp"
#include "net/ipv4.hpp"

namespace net
{
    std::ostream& boost_test_print_type(
        std::ostream& ostream, protocol_enumerator protocol)
    {
        using enum protocol_enumerator;

        switch (protocol)
        {
            case undefined: return ostream << "undefined";

            default: return ostream << "undefined protocol_enumerator";
        }
    }
}

BOOST_AUTO_TEST_SUITE(ipv4);

BOOST_AUTO_TEST_SUITE(endpoint);

BOOST_AUTO_TEST_SUITE(constructor);

BOOST_AUTO_TEST_CASE(default_constructor)
{
    const net::ipv4::endpoint endpoint;

    BOOST_CHECK_EQUAL(endpoint.address(), "0.0.0.0");
    BOOST_CHECK_EQUAL(endpoint.port(),            0);
}

BOOST_AUTO_TEST_SUITE(parameterized_constructor);

BOOST_AUTO_TEST_CASE(initialization_with_a_valid_address)
{
    BOOST_CHECK_NO_THROW(net::ipv4::endpoint("0.0.0.0"));

    std::error_code error;

    BOOST_REQUIRE_NO_THROW(net::ipv4::endpoint(error, "0.0.0.0"));

    BOOST_TEST(not error);
}

BOOST_AUTO_TEST_CASE(initialization_with_an_invalid_address)
{
    BOOST_CHECK_EXCEPTION(
        net::ipv4::endpoint {"a.b.c.d"}, std::system_error, [](auto exception)
        {
#ifdef NET_DEBUG_MODE__

            return std::string_view(exception.what()) ==
                "address: invalid ipv4 address";

#else

            return std::string_view(exception.what()) ==
                "invalid ipv4 address";

#endif
        }
    );

    std::error_code error;

    BOOST_REQUIRE_NO_THROW(net::ipv4::endpoint(error, "a.b.c.d"));

    BOOST_TEST(error);

    BOOST_CHECK_EQUAL(error.message(), "invalid ipv4 address");
}

BOOST_AUTO_TEST_SUITE_END(); // ipv4/endpoint/constructor/parameterized_constructor

BOOST_AUTO_TEST_SUITE_END(); // ipv4/endpoint/constructor

BOOST_AUTO_TEST_SUITE_END(); // ipv4/endpoint

BOOST_AUTO_TEST_SUITE(header);

BOOST_AUTO_TEST_CASE(from_data)
{
    const std::uint8_t buffer[] = {
        0x45, 0x0, 0x0, 0x14, // version, ihl, total length
        0x0,  0x0, 0x0, 0x0,  // identification, flags
        0x0,  0x0, 0x0, 0x0,  // ttl, protocol, header checksum
        0x0,  0x0, 0x0, 0x0,  // source address
        0x0,  0x0, 0x0, 0x0   // destination address
    };

    static_assert(sizeof(buffer) == 20);

    const auto ipv4_packet = net::ipv4::header::from_data(
        std::string_view {
            reinterpret_cast<const char*>(buffer),
            sizeof(buffer)
        }
    );

    BOOST_REQUIRE(ipv4_packet.has_value());

    const auto& [ipv4_header, ipv4_data] =
        ipv4_packet.value();

    BOOST_CHECK_EQUAL(ipv4_header.version(), 4);
    BOOST_CHECK_EQUAL(ipv4_header.header_size(), 20);
    BOOST_CHECK_EQUAL(ipv4_header.has_options(), false);
    BOOST_CHECK_EQUAL(ipv4_header.packet_size(), 20);

    BOOST_CHECK_EQUAL(
        ipv4_header.protocol(), net::protocol_enumerator::undefined);

    BOOST_TEST(ipv4_data.empty());
}

BOOST_AUTO_TEST_CASE(default_constructor)
{
    const net::ipv4::header header;

    BOOST_CHECK_EQUAL(header.version(), 4);
    BOOST_CHECK_EQUAL(header.header_size(), 20);
    BOOST_CHECK_EQUAL(header.has_options(), false);
    BOOST_CHECK_EQUAL(header.packet_size(), 20);
    BOOST_CHECK_EQUAL(header.protocol(), net::protocol_enumerator::undefined);
}

BOOST_AUTO_TEST_SUITE_END(); // ipv4/header

BOOST_AUTO_TEST_SUITE_END(); // ipv4
