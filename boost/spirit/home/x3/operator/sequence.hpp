/*=============================================================================
    Copyright (c) 2001-2013 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(SPIRIT_SEQUENCE_JAN_06_2012_1015AM)
#define SPIRIT_SEQUENCE_JAN_06_2012_1015AM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/x3/core/parser.hpp>

namespace boost { namespace spirit { namespace x3
{
    template <typename Left, typename Right>
    struct sequence : parser<sequence<Left, Right>>
    {
        typedef Left left_type;
        typedef Right right_type;
        static bool const has_attribute =
            left_type::has_attribute || right_type::has_attribute;

        sequence(Left left, Right right)
            : left(left), right(right) {}

        template <typename Iterator, typename Context, typename Attribute>
        bool parse(
            Iterator& first, Iterator const& last
          , Context& context, Attribute& attr) const
        {
            return false;
        }

        template <typename Iterator, typename Context>
        bool parse(
            Iterator& first, Iterator const& last
          , Context& context, unused_type) const
        {
            return left.parse(first, last, context, unused)
               && right.parse(first, last, context, unused);
        }

        left_type left;
        right_type right;
    };

    template <typename Left, typename Right>
    inline sequence<
        typename extension::as_parser<Left>::type
      , typename extension::as_parser<Right>::type>
    operator>>(Left const& left, Right const& right)
    {
        typedef sequence<
            typename extension::as_parser<Left>::type
          , typename extension::as_parser<Right>::type>
        result_type;

        return result_type(as_parser(left), as_parser(right));
    }

    //~ namespace detail
    //~ {
        //~ template <typename Parser>
        //~ struct num_elements
        //~ {
            //~ static int const value = Parser::has_attribute;
        //~ };

        //~ template <typename L, typename R>
        //~ struct num_elements<sequence<L, R>>
        //~ {
            //~ static int const value =
                //~ num_elements<Left>::value + num_elements<Right>::value;
        //~ };

        //~ template <typename Parser, typename Attribute>
        //~ struct partition_attribute
        //~ {

        //~ };
    //~ }
}}}

#endif
