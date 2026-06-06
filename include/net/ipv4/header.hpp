#pragma once

#include <string_view>
#include <optional>
#include <utility>
#include <string>

#include <cstdint>

#include "net/protocol_enumerator.hpp"

namespace net::ipv4
{
    struct header final
    {
        std::uint8_t        version_and_ihl;
        std::uint8_t        type_of_service;
        std::uint16_t       total_length;
        std::uint16_t       identification;
        std::uint16_t       flags_and_fragment_offset;
        std::uint8_t        time_to_live;
        protocol_enumerator protocol;
        std::uint16_t       header_checksum;
        std::uint32_t       source_address;
        std::uint32_t       destination_address;

        static constexpr std::size_t maximum_header_size = 60;
        static constexpr std::size_t minimum_header_size = 20;

        std::uint8_t* data() noexcept
        {
            return reinterpret_cast<std::uint8_t*>(this) + header_size();
        }

        const std::uint8_t* data() const noexcept
        {
            return reinterpret_cast<const std::uint8_t*>(this) + header_size();
        }

        static std::optional<std::pair<header, std::string>> from_data(
            std::string_view);

        constexpr bool has_options() const noexcept
        {
            return header_size() > minimum_header_size;
        }

        constexpr std::size_t header_size() const noexcept
        {
            return (version_and_ihl & 0b0000'1111) * 4;
        }

        constexpr std::size_t size() const noexcept
        {
            return total_length - header_size();
        }

        constexpr int version() const noexcept
        {
            return (version_and_ihl & 0b1111'0000) >> 4;
        }
    };
}
