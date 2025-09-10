/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#ifndef BOOST_SPIRIT_X3_STRING_STRING_HPP
#define BOOST_SPIRIT_X3_STRING_STRING_HPP

#include <boost/spirit/home/x3/string/literal_string.hpp>
#include <boost/spirit/home/x3/char/literal_char.hpp> // required for "c" -> 'c' optimization
#include <boost/spirit/home/x3/support/traits/string_traits.hpp>

#include <boost/spirit/home/x3/char_encoding/detail/encoding_warning.hpp>
#include <boost/spirit/home/x3/char_encoding/standard.hpp>
#include <boost/spirit/home/x3/char_encoding/standard_wide.hpp>

#ifdef BOOST_SPIRIT_X3_UNICODE
# include <boost/spirit/home/x3/char_encoding/unicode.hpp>
#endif

#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

namespace boost::spirit::x3
{
    namespace standard
    {
        inline namespace helpers
        {
            template <traits::CppStringLike<char> T>
            [[nodiscard]] constexpr literal_string<traits::maybe_owning_string<T>, char_encoding::standard>
            string(T&& string_like)
                noexcept(std::is_nothrow_constructible_v<literal_string<traits::maybe_owning_string<T>, char_encoding::standard>, T>)
            {
                return {std::forward<T>(string_like)};
            }

            // Optimize `literal_string{'c'}` into `literal_char{'c'}`
            [[nodiscard]] constexpr literal_char<char_encoding::standard, std::basic_string<char>>
            string(char ch) noexcept
            {
                return { ch };
            }

            // Optimize `literal_string{"c"}` into `literal_char{'c'}`
            [[nodiscard]] constexpr literal_char<char_encoding::standard, std::basic_string<char>>
            string(traits::X3VagueArrayOf2Chars<char> auto const& ch) noexcept
            {
                return { ch[0] };
            }

            template <traits::CppStringLike<char> T>
            [[nodiscard]] constexpr literal_string<traits::maybe_owning_string<T>, char_encoding::standard, unused_type>
            lit(T&& string_like)
                noexcept(std::is_nothrow_constructible_v<literal_string<traits::maybe_owning_string<T>, char_encoding::standard, unused_type>, T>)
            {
                return {std::forward<T>(string_like)};
            }
        } // helpers

        template <typename T>
            requires traits::CharIncompatibleWith<T, char> || traits::StringLikeIncompatibleWith<T, char>
        constexpr void string(T&&) = delete; // Mixing incompatible character types is not allowed
    } // standard

    using standard::helpers::string;
    using standard::helpers::lit;

#ifndef BOOST_SPIRIT_X3_NO_STANDARD_WIDE
    namespace standard_wide
    {
        inline namespace helpers
        {
            template <traits::CppStringLike<wchar_t> T>
            [[nodiscard]] constexpr literal_string<traits::maybe_owning_string<T>, char_encoding::standard_wide>
            string(T&& string_like)
                noexcept(std::is_nothrow_constructible_v<literal_string<traits::maybe_owning_string<T>, char_encoding::standard_wide>, T>)
            {
                return {std::forward<T>(string_like)};
            }

            // Optimize `literal_string{'c'}` into `literal_char{'c'}`
            [[nodiscard]] constexpr literal_char<char_encoding::standard_wide, std::basic_string<wchar_t>>
            string(wchar_t ch) noexcept
            {
                return { ch };
            }

            // Optimize `literal_string{L"c"}` into `literal_char{L'c'}`
            [[nodiscard]] constexpr literal_char<char_encoding::standard_wide, std::basic_string<wchar_t>>
            string(traits::X3VagueArrayOf2Chars<wchar_t> auto const& ch) noexcept
            {
                return { ch[0] };
            }

            template <traits::CppStringLike<wchar_t> T>
            [[nodiscard]] constexpr literal_string<traits::maybe_owning_string<T>, char_encoding::standard_wide, unused_type>
            lit(T&& string_like)
                noexcept(std::is_nothrow_constructible_v<literal_string<traits::maybe_owning_string<T>, char_encoding::standard_wide, unused_type>, T>)
            {
                return {std::forward<T>(string_like)};
            }
        } // helpers

        template <typename T>
            requires traits::CharIncompatibleWith<T, wchar_t> || traits::StringLikeIncompatibleWith<T, wchar_t>
        constexpr void string(T&&) = delete; // Mixing incompatible character types is not allowed
    } // standard_wide

    using standard_wide::helpers::string;
    using standard_wide::helpers::lit;
#endif

#ifdef BOOST_SPIRIT_X3_UNICODE
    namespace unicode
    {
        inline namespace helpers
        {
            // TODO: add `char8_t` and `char16_t` overloads
            template <traits::CppStringLike<char32_t> T>
            [[nodiscard]] constexpr literal_string<traits::maybe_owning_string<T>, char_encoding::unicode>
            string(T&& string_like)
                noexcept(std::is_nothrow_constructible_v<literal_string<traits::maybe_owning_string<T>, char_encoding::unicode>, T>)
            {
                return {std::forward<T>(string_like)};
            }

            // Optimize `literal_string{'c'}` into `literal_char{'c'}`
            [[nodiscard]] constexpr literal_char<char_encoding::unicode, std::basic_string<char32_t>>
            string(char32_t ch) noexcept
            {
                return { ch };
            }

            // Optimize `literal_string{U"c"}` into `literal_char{U'c'}`
            [[nodiscard]] constexpr literal_char<char_encoding::unicode, std::basic_string<char32_t>>
            string(traits::X3VagueArrayOf2Chars<char32_t> auto const& ch) noexcept
            {
                return { ch[0] };
            }

            template <traits::CppStringLike<char32_t> T>
            [[nodiscard]] constexpr literal_string<traits::maybe_owning_string<T>, char_encoding::unicode, unused_type>
            lit(T&& string_like)
                noexcept(std::is_nothrow_constructible_v<literal_string<traits::maybe_owning_string<T>, char_encoding::unicode, unused_type>, T>)
            {
                return {std::forward<T>(string_like)};
            }
        } // helpers

        template <typename T>
            requires traits::CharIncompatibleWith<T, char32_t> || traits::StringLikeIncompatibleWith<T, char32_t>
        constexpr void string(T&&) = delete; // Mixing incompatible character types is not allowed
    }

    using unicode::helpers::string;
    using unicode::helpers::lit;
#endif

    namespace [[deprecated(BOOST_SPIRIT_X3_WRONG_ENCODING_ASSUMPTION_WARNING)]] ascii
    {
        template <traits::CppStringLike<char> T>
        [[nodiscard]] constexpr literal_string<traits::maybe_owning_string<T>, char_encoding::ascii>
        string(T&& string_like)
            noexcept(std::is_nothrow_constructible_v<literal_string<traits::maybe_owning_string<T>, char_encoding::ascii>, T>)
        {
            return {std::forward<T>(string_like)};
        }

        // Optimize `literal_string{'c'}` into `literal_char{'c'}`
        [[nodiscard]] constexpr literal_char<char_encoding::ascii, std::basic_string<char>>
        string(char ch) noexcept
        {
            return { ch };
        }

        // Optimize `literal_string{"c"}` into `literal_char{'c'}`
        [[nodiscard]] constexpr literal_char<char_encoding::ascii, std::basic_string<char>>
        string(traits::X3VagueArrayOf2Chars<char> auto const& ch) noexcept
        {
            return { ch[0] };
        }

        template <traits::CppStringLike<char> T>
        [[nodiscard]] constexpr literal_string<traits::maybe_owning_string<T>, char_encoding::ascii, unused_type>
        lit(T&& string_like)
            noexcept(std::is_nothrow_constructible_v<literal_string<traits::maybe_owning_string<T>, char_encoding::ascii, unused_type>, T>)
        {
            return {std::forward<T>(string_like)};
        }

        template <typename T>
            requires traits::CharIncompatibleWith<T, char> || traits::StringLikeIncompatibleWith<T, char>
        constexpr void string(T&&) = delete; // Mixing incompatible character types is not allowed
    } // ascii

    namespace [[deprecated(BOOST_SPIRIT_X3_WRONG_ENCODING_ASSUMPTION_WARNING)]] iso8859_1
    {
        template <traits::CppStringLike<char> T>
        [[nodiscard]] constexpr literal_string<traits::maybe_owning_string<T>, char_encoding::iso8859_1>
        string(T&& string_like)
            noexcept(std::is_nothrow_constructible_v<literal_string<traits::maybe_owning_string<T>, char_encoding::iso8859_1>, T>)
        {
            return {std::forward<T>(string_like)};
        }

        // Optimize `literal_string{'c'}` into `literal_char{'c'}`
        [[nodiscard]] constexpr literal_char<char_encoding::iso8859_1, std::basic_string<char>>
        string(char ch) noexcept
        {
            return { ch };
        }

        // Optimize `literal_string{"c"}` into `literal_char{'c'}`
        [[nodiscard]] constexpr literal_char<char_encoding::iso8859_1, std::basic_string<char>>
        string(traits::X3VagueArrayOf2Chars<char> auto const& ch) noexcept
        {
            return { ch[0] };
        }

        template <traits::CppStringLike<char> T>
        [[nodiscard]] constexpr literal_string<traits::maybe_owning_string<T>, char_encoding::iso8859_1, unused_type>
        lit(T&& string_like)
            noexcept(std::is_nothrow_constructible_v<literal_string<traits::maybe_owning_string<T>, char_encoding::iso8859_1, unused_type>, T>)
        {
            return {std::forward<T>(string_like)};
        }

        template <typename T>
            requires traits::CharIncompatibleWith<T, char> || traits::StringLikeIncompatibleWith<T, char>
        constexpr void string(T&&) = delete; // Mixing incompatible character types is not allowed
    } // iso8859_1

    namespace extension
    {
        template <traits::CharLike CharT, std::size_t N>
        struct as_parser<CharT[N]>
        {
            using type = literal_string<std::basic_string_view<CharT>, traits::char_encoding_for<CharT>, unused_type>;
            using value_type = type;

            [[nodiscard]] static constexpr type call(CharT const* s)
            {
                return type(s);
            }
        };

        template <traits::CharLike CharT, std::size_t N>
        struct as_parser<CharT const[N]> : as_parser<CharT[N]> {};

        template <traits::CharLike CharT>
        struct as_parser<CharT const*>
        {
            using type = literal_string<std::basic_string_view<CharT>, traits::char_encoding_for<CharT>, unused_type>;
            using value_type = type;

            [[nodiscard]] static constexpr type call(CharT const* s)
            {
                return type(std::basic_string_view<CharT>{s});
            }
        };

        template <traits::CharLike CharT>
        struct as_parser<std::basic_string<CharT>>
        {
            using type = literal_string<std::basic_string<CharT>, traits::char_encoding_for<CharT>, unused_type>;
            using value_type = type;

            template <typename T>
            [[nodiscard]] static constexpr type call(T&& str)
                noexcept(std::is_nothrow_constructible_v<type, T>)
            {
                return type(std::forward<T>(str));
            }
        };

        template <traits::CharLike CharT>
        struct as_parser<std::basic_string_view<CharT>>
        {
            using type = literal_string<std::basic_string_view<CharT>, traits::char_encoding_for<CharT>, unused_type>;
            using value_type = type;

            template <typename T>
            [[nodiscard]] static constexpr type call(T&& str)
                noexcept(std::is_nothrow_constructible_v<type, T>)
            {
                return type(std::forward<T>(str));
            }
        };
    } // extension

} // boost::spirit::x3

#endif
