/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_SPIRIT_X3_CHAR_APRIL_16_2006_1051AM)
#define BOOST_SPIRIT_X3_CHAR_APRIL_16_2006_1051AM

#include <boost/spirit/home/x3/char/any_char.hpp>
#include <boost/spirit/home/x3/char/literal_char.hpp>
#include <boost/spirit/home/x3/support/traits/string_traits.hpp>

#include <boost/spirit/home/x3/char_encoding/detail/encoding_warning.hpp>
#include <boost/spirit/home/x3/char_encoding/ascii.hpp> // deprecated
#include <boost/spirit/home/x3/char_encoding/iso8859_1.hpp> // deprecated
#include <boost/spirit/home/x3/char_encoding/standard.hpp>

#ifndef BOOST_SPIRIT_X3_NO_STANDARD_WIDE
# include <boost/spirit/home/x3/char_encoding/standard_wide.hpp>
#endif

#ifdef BOOST_SPIRIT_X3_UNICODE
# include <boost/spirit/home/x3/char_encoding/unicode.hpp>
#endif

#include <type_traits>

namespace boost::spirit::x3
{
    namespace standard
    {
        inline constexpr any_char<char_encoding::standard> char_{};

        inline namespace helpers
        {
            [[nodiscard]] constexpr literal_char<char_encoding::standard, unused_type>
            lit(char ch) noexcept
            {
                return { ch };
            }

            [[nodiscard]] constexpr literal_char<char_encoding::standard, unused_type>
            lit(traits::X3VagueArrayOf2Chars<char> auto const& ch) noexcept
            {
                return { ch[0] };
            }
        } // helpers

        // If you see "no matching overload" on string literals (e.g. `"foo"`),
        // you may need to include `x3/string/literal_string.hpp`.
        // If you still see errors after the inclusion, that might be due to
        // mixing incompatible string literals. Don't do that.

        constexpr void lit(traits::CharIncompatibleWith<char> auto const*) = delete; // Mixing incompatible character types is not allowed
        constexpr void lit(traits::CharIncompatibleWith<char> auto) = delete; // Mixing incompatible character types is not allowed
    }

    inline constexpr auto const& char_ = standard::char_; // TODO: this can't overload other character types
    using standard::helpers::lit;

#ifndef BOOST_SPIRIT_X3_NO_STANDARD_WIDE
    namespace standard_wide
    {
        inline constexpr any_char<char_encoding::standard_wide> char_{};

        inline namespace helpers
        {
            [[nodiscard]] constexpr literal_char<char_encoding::standard_wide, unused_type>
            lit(wchar_t ch) noexcept
            {
                return { ch };
            }

            [[nodiscard]] constexpr literal_char<char_encoding::standard_wide, unused_type>
            lit(traits::X3VagueArrayOf2Chars<wchar_t> auto const& ch) noexcept
            {
                return { ch[0] };
            }
        } // helpers

        constexpr void lit(traits::CharIncompatibleWith<wchar_t> auto const*) = delete; // Mixing incompatible character types is not allowed
        constexpr void lit(traits::CharIncompatibleWith<wchar_t> auto) = delete; // Mixing incompatible character types is not allowed
    }

    using standard_wide::helpers::lit;
#endif

#ifdef BOOST_SPIRIT_X3_UNICODE
    namespace unicode
    {
        inline constexpr any_char<char_encoding::unicode> char_{};

        inline namespace helpers
        {
            // TODO: add `char8_t` and `char16_t` overloads

            [[nodiscard]] constexpr literal_char<char_encoding::unicode, unused_type>
            lit(char32_t ch) noexcept
            {
                return { ch };
            }

            [[nodiscard]] constexpr literal_char<char_encoding::unicode, unused_type>
            lit(traits::X3VagueArrayOf2Chars<char32_t> auto const& ch) noexcept
            {
                return { ch[0] };
            }
        } // helpers

        constexpr void lit(traits::CharIncompatibleWith<char32_t> auto const*) = delete; // Mixing incompatible character types is not allowed
        constexpr void lit(traits::CharIncompatibleWith<char32_t> auto) = delete; // Mixing incompatible character types is not allowed
    }

    using unicode::helpers::lit;
#endif

    namespace [[deprecated(BOOST_SPIRIT_X3_WRONG_ENCODING_ASSUMPTION_WARNING)]] ascii
    {
        inline constexpr any_char<char_encoding::ascii> char_{};

        [[nodiscard]] constexpr literal_char<char_encoding::ascii, unused_type>
        lit(char ch) noexcept
        {
            return { ch };
        }

        constexpr void lit(traits::CharIncompatibleWith<char> auto const*) = delete; // Mixing incompatible character types is not allowed
        constexpr void lit(traits::CharIncompatibleWith<char> auto) = delete; // Mixing incompatible character types is not allowed
    }

    namespace [[deprecated(BOOST_SPIRIT_X3_WRONG_ENCODING_ASSUMPTION_WARNING)]] iso8859_1
    {
        inline constexpr any_char<char_encoding::iso8859_1> char_{};

        [[nodiscard]] constexpr literal_char<char_encoding::iso8859_1, unused_type>
        lit(char ch) noexcept
        {
            return { ch };
        }

        constexpr void lit(traits::CharIncompatibleWith<char> auto const*) = delete; // Mixing incompatible character types is not allowed
        constexpr void lit(traits::CharIncompatibleWith<char> auto) = delete; // Mixing incompatible character types is not allowed
    }

    namespace extension
    {
        template <>
        struct as_parser<char>
        {
            using type = literal_char<char_encoding::standard, unused_type>;
            using value_type = type;

            [[nodiscard]] static constexpr type call(char ch) noexcept
            {
                return { ch };
            }
        };

#ifndef BOOST_SPIRIT_X3_NO_STANDARD_WIDE
        template <>
        struct as_parser<wchar_t>
        {
            using type = literal_char<char_encoding::standard_wide, unused_type>;
            using value_type = type;

            [[nodiscard]] static constexpr type call(wchar_t ch) noexcept
            {
                return { ch };
            }
        };
#endif

        template <>
        struct as_parser<char [2]>
        {
            using type = literal_char<char_encoding::standard, unused_type>;
            using value_type = type;

            [[nodiscard]] static constexpr type call(char const ch[]) noexcept
            {
                return { ch[0] };
            }
        };

#ifndef BOOST_SPIRIT_X3_NO_STANDARD_WIDE
        template <>
        struct as_parser<wchar_t [2]>
        {
            using type = literal_char<char_encoding::standard_wide, unused_type>;
            using value_type = type;

            [[nodiscard]] static constexpr type call(wchar_t const ch[]) noexcept
            {
                return { ch[0] };
            }
        };
#endif
    }

} // boost::spirit::x3

#endif
