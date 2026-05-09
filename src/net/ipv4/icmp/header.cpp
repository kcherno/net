#include <string_view>
#include <stdexcept>
#include <string>

#include "net/ipv4/icmp/header.hpp"

using namespace net::ipv4::icmp;

std::string make_icmp_message(const header& header, std::string_view data)
{
    switch (header.type)
    {
        default:
        {
            throw std::runtime_error {
                "make_icmp_message: Undefined net::ipv4::icmp::header::type"
            };
        }
    }
}

