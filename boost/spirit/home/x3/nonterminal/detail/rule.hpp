/*=============================================================================
    Copyright (c) 2001-2013 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_SPIRIT_DETAIL_RULE_JAN_08_2012_0326PM)
#define BOOST_SPIRIT_DETAIL_RULE_JAN_08_2012_0326PM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/x3/core/parser.hpp>
#include <boost/spirit/home/support/traits/make_attribute.hpp>
#include <boost/spirit/home/x3/nonterminal/detail/transform_attribute.hpp>

namespace boost { namespace spirit { namespace x3 { namespace detail
{
    template <typename Attribute>
    struct parse_rule
    {
        template <typename RHS, typename Iterator, typename Context, typename ActualAttribute>
        static bool call(RHS const& rhs, Iterator& first, Iterator const& last
          , Context& context, ActualAttribute& attr)
        {
            typedef traits::make_attribute<Attribute, ActualAttribute> make_attribute;

            // do down-stream transformation, provides attribute for
            // rhs parser
            typedef traits::transform_attribute<
                typename make_attribute::type, Attribute, parser_id>
            transform;

            typename make_attribute::type made_attr = make_attribute::call(attr);
            typename transform::type attr_ = transform::pre(made_attr);

            if (rhs.parse(first, last, context, attr_))
            {
                // do up-stream transformation, this integrates the results
                // back into the original attribute value, if appropriate
                traits::post_transform(attr, attr_);
                return true;
            }
            return false;
        }
    };
}}}}

#endif
