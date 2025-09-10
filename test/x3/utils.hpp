/*=============================================================================
    Copyright (c) 2019 Nikita Kniazev
    Copyright (c) 2025 Nana Sakisaka

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_TEST_X3_UTILS_HPP)
#define BOOST_SPIRIT_TEST_X3_UTILS_HPP

#include <boost/spirit/home/x3/core/parser.hpp>
#include <boost/spirit/home/x3/support/traits/move_to.hpp>

#include <iterator>

struct move_only
{
    move_only() = default;
    move_only(move_only&&) = default;
    move_only& operator=(move_only&&) = default;
};

template <typename T>
struct synth_parser : boost::spirit::x3::parser<synth_parser<T>>
{
    using attribute_type = T;

    static constexpr bool has_attribute = true;
    static constexpr bool handles_container = false;

    template <std::forward_iterator It, std::sentinel_for<It> Se, typename Context, typename RContext, typename Attribute>
    [[nodiscard]] constexpr bool parse(
        It& iter, Se const& last, Context const&,
        RContext&, Attribute& attr
    ) const
    {
        if (iter != last && *iter == 's') {
            ++iter;
            boost::spirit::x3::traits::move_to(attribute_type{}, attr);
            return true;
        }
        return false;
    }
};

template <typename T>
constexpr synth_parser<T> synth{};

constexpr synth_parser<move_only> synth_move_only{};

#endif
