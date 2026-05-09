#pragma once

#include <string_view>
#include <string>

#include <cstdint>

namespace net::ipv4::icmp
{
    struct header final
    {
        enum class type_enumerator : std::uint8_t
        {
            echo_replay = 0,
            echo        = 8
        };

        type_enumerator type;
        std::uint8_t    code;
        std::uint16_t   checksum;

        union
        {
            struct
            {

                std::uint16_t identifier;
                std::uint16_t sequence_number;

            } echo_message;
        };

        inline static constexpr std::size_t echo_message_header_size = 8;
    };

    std::string make_icmp_message(header::type_enumerator, std::string_view);
}
