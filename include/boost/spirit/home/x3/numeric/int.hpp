/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#ifndef BOOST_SPIRIT_X3_INT_APR_17_2006_0830AM
#define BOOST_SPIRIT_X3_INT_APR_17_2006_0830AM

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
    struct int_parser : parser<int_parser<T, Radix, MinDigits, MaxDigits>>
    {
        // check template parameter 'Radix' for validity
        static_assert(
            (Radix == 2 || Radix == 8 || Radix == 10 || Radix == 16),
            "Unsupported Radix"
        );

        using attribute_type = T;
        static constexpr bool has_attribute = true;

        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Context, typename Attribute>
        [[nodiscard]] constexpr bool parse(
            It& first, Se const& last,
            Context const& context, unused_type, Attribute& attr
        ) const noexcept(
            noexcept(x3::skip_over(first, last, context)) &&
            noexcept(extract_int<T, Radix, MinDigits, MaxDigits>::call(first, last, attr))
        )
        {
            x3::skip_over(first, last, context);
            return extract_int<T, Radix, MinDigits, MaxDigits>::call(first, last, attr);
        }
    };

    inline namespace cpos
    {
        using short_type = int_parser<short>;
        inline constexpr short_type short_{};

        using int_type = int_parser<int>;
        inline constexpr int_type int_{};

        using long_type = int_parser<long>;
        inline constexpr long_type long_{};

        using long_long_type = int_parser<long long>;
        inline constexpr long_long_type long_long{};


        using int8_type = int_parser<std::int8_t>;
        inline constexpr int8_type int8{};

        using int16_type = int_parser<std::int16_t>;
        inline constexpr int16_type int16{};

        using int32_type = int_parser<std::int32_t>;
        inline constexpr int32_type int32{};

        using int64_type = int_parser<std::int64_t>;
        inline constexpr int64_type int64{};

    } // cpos

} // boost::spirit::x3

#endif
