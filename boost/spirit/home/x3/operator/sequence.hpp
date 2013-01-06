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
#include <boost/spirit/home/x3/operator/detail/sequence.hpp>

namespace boost { namespace spirit { namespace x3
{
    namespace detail
    {
        template <typename L, typename R, typename Attribute>
        struct partition_attribute;
    }

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
            typedef detail::partition_attribute<Left, Right, Attribute> partition;
            typedef typename partition::l_pass l_pass;
            typedef typename partition::r_pass r_pass;

            typename partition::l_range left_seq = partition::left(attr);
            typename partition::r_range right_seq = partition::right(attr);
            typename l_pass::type l_attr = l_pass::call(left_seq);
            typename r_pass::type r_attr = r_pass::call(right_seq);

            return left.parse(first, last, context, l_attr)
               && right.parse(first, last, context, r_attr);
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
        typename extension::as_parser<Left>::value_type
      , typename extension::as_parser<Right>::value_type>
    operator>>(Left const& left, Right const& right)
    {
        typedef sequence<
            typename extension::as_parser<Left>::value_type
          , typename extension::as_parser<Right>::value_type>
        result_type;

        return result_type(as_parser(left), as_parser(right));
    }
}}}

#endif
