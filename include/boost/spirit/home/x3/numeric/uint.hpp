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

#define BOOST_SPIRIT_X3_UINT_PARSER(uint_type, name)                            \
    using name##type = uint_parser<uint_type>;                                  \
    inline constexpr name##type name{};

    BOOST_SPIRIT_X3_UINT_PARSER(unsigned long, ulong_)
    BOOST_SPIRIT_X3_UINT_PARSER(unsigned short, ushort_)
    BOOST_SPIRIT_X3_UINT_PARSER(unsigned int, uint_)
    BOOST_SPIRIT_X3_UINT_PARSER(unsigned long long, ulong_long)

    BOOST_SPIRIT_X3_UINT_PARSER(std::uint8_t, uint8)
    BOOST_SPIRIT_X3_UINT_PARSER(std::uint16_t, uint16)
    BOOST_SPIRIT_X3_UINT_PARSER(std::uint32_t, uint32)
    BOOST_SPIRIT_X3_UINT_PARSER(std::uint64_t, uint64)

#undef BOOST_SPIRIT_X3_UINT_PARSER

#define BOOST_SPIRIT_X3_UINT_PARSER(uint_type, radix, name)                     \
    using name##type = uint_parser<uint_type, radix>;                           \
    inline constexpr name##type name{};

    BOOST_SPIRIT_X3_UINT_PARSER(unsigned, 2, bin)
    BOOST_SPIRIT_X3_UINT_PARSER(unsigned, 8, oct)
    BOOST_SPIRIT_X3_UINT_PARSER(unsigned, 16, hex)

#undef BOOST_SPIRIT_X3_UINT_PARSER

} // boost::spirit::x3

#endif
