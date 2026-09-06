#pragma once

#include <concepts>

namespace net::name_requirement
{
    template<typename T>
    concept Endpoint = requires(T a, const T b)
    {
        typename T::native_handle_type;
        typename T::size_type;

        { a.data() } noexcept ->
            std::same_as<typename T::native_handle_type*>;

        { b.data() } noexcept ->
            std::same_as<const typename T::native_handle_type*>;

        { a.size() } noexcept -> std::same_as<typename T::size_type>;
        { b.size() } noexcept -> std::same_as<typename T::size_type>;
    };
}
