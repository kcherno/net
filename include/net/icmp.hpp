#pragma once

#include <string_view>
#include <optional>
#include <utility>
#include <string>

#include <cstdint>

#include <sys/socket.h>

#include "generic/basic_datagram_socket.hpp"

#include "protocol_enumerator.hpp"
#include "ipv4.hpp"

namespace net
{
    class icmp final
    {
    public:

        using domain_type = ipv4;

        struct header final
        {
            enum class type_enumerator : std::uint8_t
            {
                echo_reply = 0,
                echo       = 8
            };

            type_enumerator type;
            std::uint8_t    code;
            std::uint16_t   checksum = {};

            union
            {
                struct
                {

                    std::uint16_t identifier;
                    std::uint16_t sequence_number;

                } echo_message;
            };

            inline static constexpr std::size_t echo_message_header_size = 8;

            static std::optional<std::pair<header, std::string>> from_data(
                std::string_view);
        };

        icmp() = delete;

        static std::string make_icmp_message(const header&, std::string_view);

        static constexpr protocol_enumerator protocol() noexcept
        {
            return protocol_enumerator::icmp;
        }

        static constexpr int type() noexcept
        {
            return SOCK_RAW;
        }

        using endpoint = domain_type::endpoint;
        using socket   = generic::basic_datagram_socket<icmp>;
    };
}
