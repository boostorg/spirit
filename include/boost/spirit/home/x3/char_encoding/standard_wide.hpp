/*=============================================================================
    Copyright (c) 2001-2011 Hartmut Kaiser
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#ifndef BOOST_SPIRIT_X3_CHAR_ENCODING_STANDARD_WIDE_HPP
#define BOOST_SPIRIT_X3_CHAR_ENCODING_STANDARD_WIDE_HPP

#include <boost/assert.hpp>

#include <string>
#include <type_traits>

#include <cstdint>
#include <cwctype>

namespace boost::spirit::x3::char_encoding
{
    // Test characters for specified conditions (using std wchar_t functions)

    struct standard_wide
    {
        using char_type = wchar_t;
        using classify_type = wchar_t;

        template <typename Char>
        [[nodiscard]] static constexpr typename std::char_traits<Char>::int_type
        to_int_type(Char ch) noexcept
        {
            return std::char_traits<Char>::to_int_type(ch);
        }

        template <typename Char>
        [[nodiscard]] static constexpr Char
        to_char_type(typename std::char_traits<Char>::int_type ch) noexcept
        {
            return std::char_traits<Char>::to_char_type(ch);
        }

        [[nodiscard]] static constexpr bool
        ischar(int ch) noexcept
        {
            static_assert(
                sizeof(wchar_t) == 1 || sizeof(wchar_t) == 2 || sizeof(wchar_t) == 4,
                "Unsupported wchar_t size"
            );
            constexpr unsigned mask =
                sizeof(wchar_t) == 4 ? 0xffffffffu :
                sizeof(wchar_t) == 2 ? 0xffffu :
                0xffu;

            // we have to watch out for sign extensions (casting is there to
            // silence certain compilers complaining about signed/unsigned
            // mismatch)
            return (
                std::size_t(0) ==
                    std::size_t(ch & ~mask) ||
                std::size_t(~0) ==
                    std::size_t(ch | mask)
            ) != 0;     // any wchar_t, but no other bits set
        }

        [[nodiscard]] static bool // TODO: constexpr
        isalnum(wchar_t ch) noexcept
        {
            return std::iswalnum(standard_wide::to_int_type(ch)) != 0;
        }

        [[nodiscard]] static bool // TODO: constexpr
        isalpha(wchar_t ch) noexcept
        {
            return std::iswalpha(standard_wide::to_int_type(ch)) != 0;
        }

        [[nodiscard]] static bool // TODO: constexpr
        iscntrl(wchar_t ch) noexcept
        {
            return std::iswcntrl(standard_wide::to_int_type(ch)) != 0;
        }

        [[nodiscard]] static bool // TODO: constexpr
        isdigit(wchar_t ch) noexcept
        {
            return std::iswdigit(standard_wide::to_int_type(ch)) != 0;
        }

        [[nodiscard]] static bool // TODO: constexpr
        isgraph(wchar_t ch) noexcept
        {
            return std::iswgraph(standard_wide::to_int_type(ch)) != 0;
        }

        [[nodiscard]] static bool // TODO: constexpr
        islower(wchar_t ch) noexcept
        {
            return std::iswlower(standard_wide::to_int_type(ch)) != 0;
        }

        [[nodiscard]] static bool // TODO: constexpr
        isprint(wchar_t ch) noexcept
        {
            return std::iswprint(standard_wide::to_int_type(ch)) != 0;
        }

        [[nodiscard]] static bool // TODO: constexpr
        ispunct(wchar_t ch) noexcept
        {
            return std::iswpunct(standard_wide::to_int_type(ch)) != 0;
        }

        [[nodiscard]] static bool // TODO: constexpr
        isspace(wchar_t ch) noexcept
        {
            return std::iswspace(standard_wide::to_int_type(ch)) != 0;
        }

        [[nodiscard]] static bool // TODO: constexpr
        isupper(wchar_t ch) noexcept
        {
            return std::iswupper(standard_wide::to_int_type(ch)) != 0;
        }

        [[nodiscard]] static bool // TODO: constexpr
        isxdigit(wchar_t ch) noexcept
        {
            return std::iswxdigit(standard_wide::to_int_type(ch)) != 0;
        }

        [[nodiscard]] static constexpr bool
        (isblank)(wchar_t ch) noexcept
        {
            return (ch == L' ' || ch == L'\t');
        }

        // Simple character conversions

        [[nodiscard]] static wchar_t // TODO: constexpr
        tolower(wchar_t ch) noexcept
        {
            return standard_wide::isupper(ch) ?
                standard_wide::to_char_type<wchar_t>(std::towlower(standard_wide::to_int_type(ch))) : ch;
        }

        [[nodiscard]] static wchar_t // TODO: constexpr
        toupper(wchar_t ch) noexcept
        {
            return std::islower(ch) ?
                standard_wide::to_char_type<wchar_t>(std::towupper(standard_wide::to_int_type(ch))) : ch;
        }

        [[nodiscard]] static constexpr std::uint32_t
        toucs4(wchar_t ch) noexcept
        {
            return static_cast<std::make_unsigned_t<wchar_t>>(ch);
        }
    };
} // boost::spirit::x3::char_encoding

#endif
