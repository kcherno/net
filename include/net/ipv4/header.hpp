#pragma once

#include <string_view>
#include <optional>
#include <utility>
#include <string>

#include <cstdint>

#include "net/protocol_enumerator.hpp"

namespace net::ipv4
{
    class header final
    {
    public:

        static constexpr std::size_t maximum_header_size = 60;
        static constexpr std::size_t minimum_header_size = 20;

        static std::optional<std::pair<header, std::string>> from_data(
            std::string_view);

        constexpr header() noexcept :
            version_and_ihl_           {0b0100'0101},
            type_of_service_           {},
            total_length_              {},
            identification_            {},
            flags_and_fragment_offset_ {},
            time_to_live_              {},
            protocol_                  {},
            header_checksum_           {},
            source_address_            {},
            destination_address_       {}
        {}

        constexpr bool has_options() const noexcept
        {
            return header_size() > minimum_header_size;
        }

        constexpr std::size_t header_size() const noexcept
        {
            return (version_and_ihl_ & 0b0000'1111) * 4;
        }

        constexpr protocol_enumerator protocol() const noexcept
        {
            return protocol_;
        }

        std::string to_string() const noexcept
        {
            std::string string {header_size(), '\0'};

            auto pointer = reinterpret_cast<header*>(string.data());

            *pointer = this;

            return string;
        }

        constexpr int version() const noexcept
        {
            return (version_and_ihl_ & 0b1111'0000) >> 4;
        }

    private:

        std::uint8_t        version_and_ihl_;
        std::uint8_t        type_of_service_;
        std::uint16_t       total_length_;
        std::uint16_t       identification_;
        std::uint16_t       flags_and_fragment_offset_;
        std::uint8_t        time_to_live_;
        protocol_enumerator protocol_;
        std::uint16_t       header_checksum_;
        std::uint32_t       source_address_;
        std::uint32_t       destination_address_;
    };
}
