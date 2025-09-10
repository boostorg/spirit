/*=============================================================================
    Copyright (c) 2001-2011 Hartmut Kaiser
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#ifndef BOOST_SPIRIT_X3_CHAR_ENCODING_STANDARD_HPP
#define BOOST_SPIRIT_X3_CHAR_ENCODING_STANDARD_HPP

#include <boost/assert.hpp>

#include <cstdint>
#include <cctype>
#include <climits>

namespace boost::spirit::x3::char_encoding
{
    // Test characters for specified conditions (using std functions)
    struct standard
    {
        using char_type = char;
        using classify_type = unsigned char;

        [[nodiscard]] static constexpr bool
        isascii_(int ch) noexcept
        {
            return 0 == (ch & ~0x7f);
        }

        [[nodiscard]] static constexpr bool
        ischar(int ch) noexcept
        {
            // uses all 8 bits
            // we have to watch out for sign extensions
            return (0 == (ch & ~0xff) || ~0 == (ch | 0xff)) != 0;
        }

        // *** Note on assertions: The precondition is that the calls to
        // these functions do not violate the required range of ch (int)
        // which is that strict_ischar(ch) should be true. It is the
        // responsibility of the caller to make sure this precondition is not
        // violated.

        [[nodiscard]] static constexpr bool
        strict_ischar(int ch) noexcept
        {
            // ch should be representable as an unsigned char
            return ch >= 0 && ch <= UCHAR_MAX;
        }

        [[nodiscard]] static bool // TODO: constexpr
        isalnum(int ch) noexcept
        {
            BOOST_ASSERT(standard::strict_ischar(ch));
            return std::isalnum(ch) != 0;
        }

        [[nodiscard]] static bool // TODO: constexpr
        isalpha(int ch) noexcept
        {
            BOOST_ASSERT(standard::strict_ischar(ch));
            return std::isalpha(ch) != 0;
        }

        [[nodiscard]] static bool // TODO: constexpr
        isdigit(int ch) noexcept
        {
            BOOST_ASSERT(standard::strict_ischar(ch));
            return std::isdigit(ch) != 0;
        }

        [[nodiscard]] static bool // TODO: constexpr
        isxdigit(int ch) noexcept
        {
            BOOST_ASSERT(standard::strict_ischar(ch));
            return std::isxdigit(ch) != 0;
        }

        [[nodiscard]] static bool // TODO: constexpr
        iscntrl(int ch) noexcept
        {
            BOOST_ASSERT(standard::strict_ischar(ch));
            return std::iscntrl(ch) != 0;
        }

        [[nodiscard]] static bool // TODO: constexpr
        isgraph(int ch) noexcept
        {
            BOOST_ASSERT(standard::strict_ischar(ch));
            return std::isgraph(ch) != 0;
        }

        [[nodiscard]] static bool // TODO: constexpr
        islower(int ch) noexcept
        {
            BOOST_ASSERT(standard::strict_ischar(ch));
            return std::islower(ch) != 0;
        }

        [[nodiscard]] static bool // TODO: constexpr
        isprint(int ch) noexcept
        {
            BOOST_ASSERT(standard::strict_ischar(ch));
            return std::isprint(ch) != 0;
        }

        [[nodiscard]] static bool // TODO: constexpr
        ispunct(int ch) noexcept
        {
            BOOST_ASSERT(standard::strict_ischar(ch));
            return std::ispunct(ch) != 0;
        }

        [[nodiscard]] static bool // TODO: constexpr
        isspace(int ch) noexcept
        {
            BOOST_ASSERT(standard::strict_ischar(ch));
            return std::isspace(ch) != 0;
        }

        [[nodiscard]] static constexpr bool
        (isblank)(int ch) noexcept
        {
            BOOST_ASSERT(standard::strict_ischar(ch));
            return (ch == ' ' || ch == '\t');
        }

        [[nodiscard]] static bool // TODO: constexpr
        isupper(int ch) noexcept
        {
            BOOST_ASSERT(standard::strict_ischar(ch));
            return std::isupper(ch) != 0;
        }

        // Simple character conversions

        [[nodiscard]] static int // TODO: constexpr
        tolower(int ch) noexcept
        {
            BOOST_ASSERT(standard::strict_ischar(ch));
            return std::tolower(ch);
        }

        [[nodiscard]] static int // TODO: constexpr
        toupper(int ch) noexcept
        {
            BOOST_ASSERT(standard::strict_ischar(ch));
            return std::toupper(ch);
        }

        [[nodiscard]] static constexpr std::uint32_t
        toucs4(int ch) noexcept
        {
            BOOST_ASSERT(standard::strict_ischar(ch));
            return static_cast<std::uint32_t>(ch);
        }
    };
} // boost::spirit::x3::char_encoding

#endif
