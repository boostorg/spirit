/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_SPIRIT_X3_CHAR_PARSER_APR_16_2006_0906AM)
#define BOOST_SPIRIT_X3_CHAR_PARSER_APR_16_2006_0906AM

#include <boost/spirit/home/x3/core/parser.hpp>
#include <boost/spirit/home/x3/core/skip_over.hpp>
#include <boost/spirit/home/x3/support/traits/move_to.hpp>
#include <boost/spirit/home/x3/support/no_case.hpp>

#include <iterator>

namespace boost::spirit::x3
{
    template <typename Derived>
    struct char_parser : parser<Derived>
    {
        template <std::forward_iterator It, std::sentinel_for<It> Se, typename Context, typename Attribute>
        [[nodiscard]] constexpr bool parse(
            It& first, Se const& last, Context const& context, unused_type, Attribute& attr
        ) const // I (saki7) don't think this can ever be noexcept, due to the nature of the operations below
        {
            x3::skip_over(first, last, context);
            if (first != last && this->derived().test(*first, context))
            {
                x3::traits::move_to(std::iter_value_t<It>{*first}, attr);
                ++first;
                return true;
            }
            return false;
        }
    };
} // boost::spirit::x3

#endif
