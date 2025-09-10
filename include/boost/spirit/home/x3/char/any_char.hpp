/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_SPIRIT_X3_ANY_CHAR_APRIL_16_2006_1051AM)
#define BOOST_SPIRIT_X3_ANY_CHAR_APRIL_16_2006_1051AM

#include <boost/spirit/home/x3/char/literal_char.hpp>
#include <boost/spirit/home/x3/char/char_set.hpp>

namespace boost::spirit::x3
{
    template <typename Encoding>
    struct any_char : char_parser<any_char<Encoding>>
    {
        using char_type = typename Encoding::char_type;
        using encoding = Encoding;
        using attribute_type = char_type;
        static constexpr bool has_attribute = true;

        template <typename Context>
        [[nodiscard]] static constexpr bool test(char_type ch, Context const&) noexcept
        {
            return encoding::ischar(ch);
        }

        [[nodiscard]] static constexpr literal_char<Encoding> operator()(char_type ch) noexcept
        {
            return { ch };
        }

        [[nodiscard]] static constexpr literal_char<Encoding> operator()(char_type const (&ch)[2]) noexcept
        {
            return { ch[0] };
        }

        template <std::size_t N>
        [[nodiscard]] static constexpr char_set<Encoding> operator()(char_type const (&ch)[N]) noexcept
        {
            return { ch };
        }

        [[nodiscard]] static constexpr char_range<Encoding> operator()(char_type from, char_type to) noexcept
        {
            return { from, to };
        }

        [[nodiscard]] static constexpr char_range<Encoding> operator()(char_type const (&from)[2], char_type const (&to)[2]) noexcept
        {
            return { static_cast<char_type>(from[0]), static_cast<char_type>(to[0]) };
        }

        [[nodiscard]] static char_set<Encoding> operator()(std::basic_string_view<char_type> sv) noexcept
        {
            return { std::move(sv) };
        }
    };
} // boost::spirit::x3

#endif
