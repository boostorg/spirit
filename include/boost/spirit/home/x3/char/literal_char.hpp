/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_SPIRIT_X3_LITERAL_CHAR_APRIL_16_2006_1051AM)
#define BOOST_SPIRIT_X3_LITERAL_CHAR_APRIL_16_2006_1051AM

#include <boost/spirit/home/x3/char/char_parser.hpp>
#include <boost/spirit/home/x3/support/utility/utf8.hpp>

#include <type_traits>
#include <concepts>

namespace boost::spirit::x3
{
    template <typename Encoding, typename Attribute = typename Encoding::char_type>
    struct literal_char : char_parser<literal_char<Encoding, Attribute>>
    {
        using char_type = typename Encoding::char_type;
        using encoding = Encoding;
        using attribute_type = Attribute;
        static constexpr bool has_attribute = !std::is_same_v<unused_type, attribute_type>;

        template <typename Char>
            requires std::convertible_to<Char, char_type>
        constexpr literal_char(Char ch) noexcept
            : ch(static_cast<char_type>(ch))
        {
            static_assert(std::same_as<char_type, Char>, "Mixing incompatible character types is not allowed");
        }

        template <typename Char, typename Context>
        [[nodiscard]] constexpr bool test(Char ch_, Context const& context) const noexcept
        {
            static_assert(std::same_as<char_type, Char>, "Mixing incompatible character types is not allowed");
            return x3::get_case_compare<encoding>(context)(ch, char_type(ch_)) == 0;
        }

        char_type ch;
    };

    template <typename Encoding, typename Attribute>
    struct get_info<literal_char<Encoding, Attribute>>
    {
        using result_type = std::string;
        [[nodiscard]] std::string operator()(literal_char<Encoding, Attribute> const& p) const
        {
            return '\'' + x3::to_utf8(Encoding::toucs4(p.ch)) + '\'';
        }
    };
} // boost::spirit::x3

#endif
