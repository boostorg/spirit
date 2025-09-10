/*=============================================================================
    Copyright (c) 2009 Hartmut Kaiser
    Copyright (c) 2014 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_SPIRIT_X3_BOOL_POLICIES_SEP_29_2009_0710AM)
#define BOOST_SPIRIT_X3_BOOL_POLICIES_SEP_29_2009_0710AM

#include <boost/spirit/home/x3/string/detail/string_parse.hpp>
#include <boost/spirit/home/x3/support/traits/move_to.hpp>

#include <string_view>
#include <iterator>

namespace boost::spirit::x3
{
    //  Default boolean policies
    template <typename T = bool>
    struct bool_policies
    {
        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Attribute, typename CaseCompare>
        [[nodiscard]] static constexpr bool
        parse_true(It& first, Se const& last, Attribute& attr_, CaseCompare const& case_compare)
            noexcept(std::is_nothrow_constructible_v<Attribute, T>)
        {
            using namespace std::string_view_literals;
            if (detail::string_parse("true"sv, first, last, unused, case_compare))
            {
                traits::move_to(T(true), attr_);    // result is true
                return true;
            }
            return false;
        }

        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Attribute, typename CaseCompare>
        [[nodiscard]] static constexpr bool
        parse_false(It& first, Se const& last, Attribute& attr_, CaseCompare const& case_compare)
            noexcept(std::is_nothrow_constructible_v<Attribute, T>)
        {
            using namespace std::string_view_literals;
            if (detail::string_parse("false"sv, first, last, unused, case_compare))
            {
                traits::move_to(T(false), attr_);   // result is false
                return true;
            }
            return false;
        }
    };
} // boost::spirit::x3

#endif
