/*=============================================================================
    Copyright (c) 2001-2013 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(SPIRIT_ALTERNATIVE_JAN_07_2012_1131AM)
#define SPIRIT_ALTERNATIVE_JAN_07_2012_1131AM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/x3/core/parser.hpp>
//~ #include <boost/spirit/home/x3/operator/detail/sequence.hpp>

namespace boost { namespace spirit { namespace x3
{
    template <typename Left, typename Right>
    struct alternative : parser<alternative<Left, Right>>
    {
        typedef Left left_type;
        typedef Right right_type;
        static bool const has_attribute =
            left_type::has_attribute || right_type::has_attribute;

        alternative(Left left, Right right)
            : left(left), right(right) {}

        template <typename Iterator, typename Context>
        bool parse(
            Iterator& first, Iterator const& last
          , Context& context, unused_type) const
        {
            return left.parse(first, last, context, unused)
               || right.parse(first, last, context, unused);
        }

        template <typename Iterator, typename Context, typename Attribute>
        bool parse(
            Iterator& first, Iterator const& last
          , Context& context, Attribute& attr) const
        {
            return false;
        }

        left_type left;
        right_type right;
    };

    template <typename Left, typename Right>
    inline alternative<
        typename extension::as_parser<Left>::value_type
      , typename extension::as_parser<Right>::value_type>
    operator|(Left const& left, Right const& right)
    {
        typedef alternative<
            typename extension::as_parser<Left>::value_type
          , typename extension::as_parser<Right>::value_type>
        result_type;

        return result_type(as_parser(left), as_parser(right));
    }
}}}

//~ namespace boost { namespace spirit { namespace traits
//~ {
    //~ template <typename Left, typename Right>
    //~ struct attribute_of<x3::sequence<Left, Right>>
        //~ : x3::detail::attribute_of_sequence<Left, Right> {};
//~ }}}

#endif
