/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#ifndef BOOST_SPIRIT_X3_CHAR_DETAIL_CHAR_RANGE_FUNCTIONS_HPP
#define BOOST_SPIRIT_X3_CHAR_DETAIL_CHAR_RANGE_FUNCTIONS_HPP

#include <boost/spirit/home/x3/char/detail/char_range.hpp>

#include <limits>
#include <concepts>

namespace boost::spirit::x3::detail
{
    template <typename CharT>
    [[nodiscard]] constexpr bool
    is_valid(char_range<CharT> const& range) noexcept
    {
        // test for valid ranges
        return range.first <= range.last;
    }

    template <typename CharT>
    [[nodiscard]] constexpr bool
    includes(char_range<CharT> const& range, char_range<CharT> const& other) noexcept
    {
        // see if two ranges intersect
        return (range.first <= other.first) && (range.last >= other.last);
    }

    template <typename CharT>
    [[nodiscard]] constexpr bool
    includes(char_range<CharT> const& range, CharT val) noexcept
    {
        // see if val is in range
        return (range.first <= val) && (range.last >= val);
    }

    template <typename CharT>
    [[nodiscard]] constexpr bool
    can_merge(char_range<CharT> const& range, char_range<CharT> const& other) noexcept
    {
        // see if a 'range' overlaps, or is adjacent to
        // another range 'other', so we can merge them

        using limits = std::numeric_limits<CharT>;

        CharT const decr_first =
            range.first == (limits::min)()
            ? range.first : range.first-1;

        CharT const incr_last =
            range.last == (limits::max)()
            ? range.last : range.last+1;

        return (decr_first <= other.last) && (incr_last >= other.first);
    }

    template <typename CharT>
    constexpr void
    merge(char_range<CharT>& result, char_range<CharT> const& other) noexcept
    {
        // merge two ranges
        if (result.first > other.first)
        {
            result.first = other.first;
        }
        if (result.last < other.last)
        {
            result.last = other.last;
        }
    }

    struct char_range_compare
    {
        using is_transparent = int;

        template <typename CharT>
        [[nodiscard]] constexpr bool operator()(char_range<CharT> const& x, CharT const y) const noexcept
        {
            return x.first < y;
        }

        template <typename CharT>
        [[nodiscard]] constexpr bool operator()(CharT const x, char_range<CharT> const& y) const noexcept
        {
            return x < y.first;
        }

        template <typename CharT>
        [[nodiscard]] constexpr bool operator()(char_range<CharT> const& x, char_range<CharT> const& y) const noexcept
        {
            return x.first < y.first;
        }

        // This overload is required to satsify `std::indirect_strict_weak_order`
        template <std::integral CharT> // hack: minimal constraint to avoid obvious mistakes
        [[nodiscard]] constexpr bool operator()(CharT const x, CharT const y) const noexcept
        {
            return x == y;
        }
    };
} // boost::spirit::x3::detail

#endif
