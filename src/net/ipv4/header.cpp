#include <string_view>
#include <optional>
#include <utility>
#include <string>

#include "net/ipv4/header.hpp"

#include "net/to_host_byte_order.hpp"

std::optional<std::pair<net::ipv4::header, std::string>>
net::ipv4::header::from_data(std::string_view data)
{
    if (data.size() >= minimum_header_size)
    {
        auto pointer = reinterpret_cast<const header*>(data.data());

        if (pointer->version() == 4)
        {
            header header {
                .version_and_ihl =
                    pointer->version_and_ihl,

                .type_of_service =
                    pointer->type_of_service,

                .total_length = to_host_byte_order(
                    pointer->total_length),

                .identification = to_host_byte_order(
                    pointer->identification),

                .flags_and_fragment_offset = to_host_byte_order(
                    pointer->flags_and_fragment_offset),

                .time_to_live =
                    pointer->time_to_live,

                .protocol =
                    pointer->protocol,

                .header_checksum = to_host_byte_order(
                    pointer->header_checksum),

                .source_address = to_host_byte_order(
                    pointer->source_address),

                .destination_address = to_host_byte_order(
                    pointer->destination_address)
            };

            return std::pair {
                header,

                std::string {
                    reinterpret_cast<const char*>(header.data()),
                    header.size()
                }
            };
        }
    }

    return std::nullopt;
}
