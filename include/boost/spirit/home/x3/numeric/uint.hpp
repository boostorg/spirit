/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2011 Jan Frederick Eick
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#ifndef BOOST_SPIRIT_X3_UINT_APR_17_2006_0901AM
#define BOOST_SPIRIT_X3_UINT_APR_17_2006_0901AM

#include <boost/spirit/home/x3/core/parser.hpp>
#include <boost/spirit/home/x3/core/skip_over.hpp>
#include <boost/spirit/home/x3/support/numeric_utils/extract_int.hpp>

#include <iterator>
#include <cstdint>

namespace boost::spirit::x3
{
    template <
        typename T,
        unsigned Radix = 10,
        unsigned MinDigits = 1,
        int MaxDigits = -1
    >
    struct uint_parser : parser<uint_parser<T, Radix, MinDigits, MaxDigits>>
    {
        // check template parameter 'Radix' for validity
        static_assert((Radix >= 2 && Radix <= 36), "Unsupported Radix");

        using attribute_type = T;

        static constexpr bool has_attribute = true;

        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Context, typename Attribute>
        [[nodiscard]] constexpr bool
        parse(
            It& first, Se const& last,
            Context const& context, unused_type, Attribute& attr
        ) const noexcept(
            noexcept(x3::skip_over(first, last, context)) &&
            noexcept(extract_uint<T, Radix, MinDigits, MaxDigits>::call(first, last, attr))
        )
        {
            x3::skip_over(first, last, context);
            return extract_uint<T, Radix, MinDigits, MaxDigits>::call(first, last, attr);
        }
    };

    inline namespace cpos
    {
        using ushort_type = uint_parser<unsigned short>;
        inline constexpr ushort_type ushort_{};

        using uint_type = uint_parser<unsigned int>;
        inline constexpr uint_type uint_{};

        using ulong_type = uint_parser<unsigned long>;
        inline constexpr ulong_type ulong_{};

        using ulong_long_type = uint_parser<unsigned long long>;
        inline constexpr ulong_long_type ulong_long{};


        using uint8_type = uint_parser<std::uint8_t>;
        inline constexpr uint8_type uint8{};

        using uint16_type = uint_parser<std::uint16_t>;
        inline constexpr uint16_type uint16{};

        using uint32_type = uint_parser<std::uint32_t>;
        inline constexpr uint32_type uint32{};

        using uint64_type = uint_parser<std::uint64_t>;
        inline constexpr uint64_type uint64{};


        using bin_type = uint_parser<unsigned, 2>;
        inline constexpr bin_type bin{};

        using oct_type = uint_parser<unsigned, 8>;
        inline constexpr oct_type oct{};

        using hex_type = uint_parser<unsigned, 16>;
        inline constexpr hex_type hex{};
    } // cpos

} // boost::spirit::x3

#endif
