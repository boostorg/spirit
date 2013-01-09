/*=============================================================================
    Copyright (c) 2001-2013 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_SPIRIT_RULE_JAN_08_2012_0326PM)
#define BOOST_SPIRIT_RULE_JAN_08_2012_0326PM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/x3/core/parser.hpp>
#include <boost/spirit/home/support/traits/make_attribute.hpp>
#include <boost/spirit/home/support/context.hpp>
#include <boost/spirit/home/x3/nonterminal/detail/transform_attribute.hpp>
#include <boost/type_traits/is_same.hpp>

namespace boost { namespace spirit { namespace x3
{
    template <typename RHS, typename Attribute>
    struct rule_context
    {
        Attribute val;
        RHS const& rhs;
    };

    template <typename ID, typename RHS, typename Attribute>
    struct rule_definition : parser<rule_definition<ID, RHS, Attribute>>
    {
        typedef ID id;
        typedef RHS rhs_type;
        typedef Attribute attribute_type;
        static bool const has_attribute =
            !is_same<Attribute, unused_type>::value;

        rule_definition(RHS rhs)
          : rhs(rhs) {}

        template <typename Iterator, typename Context, typename Attribute_>
        bool parse(Iterator& first, Iterator const& last
          , Context& context, Attribute_& attr) const
        {
            typedef traits::make_attribute<attribute_type, Attribute_> make_attribute;

            // do down-stream transformation, provides attribute for
            // rhs parser
            typedef traits::transform_attribute<
                typename make_attribute::type, attribute_type, parser_id>
            transform;

            typename make_attribute::type made_attr = make_attribute::call(attr);
            typedef rule_context<RHS, typename transform::type> rule_context_type;
            typedef spirit::context<ID, rule_context_type, Context> our_context_type;

            rule_context_type context_info = { transform::pre(made_attr), rhs };
            our_context_type our_context(context_info,  context);
            if (rhs.parse(first, last, our_context, context_info.val))
            {
                // do up-stream transformation, this integrates the results
                // back into the original attribute value, if appropriate
                traits::post_transform(attr, context_info.val);
                return true;
            }
            return false;
        }

        RHS rhs;
    };

    template <typename ID, typename Attribute = unused_type>
    struct rule : parser<rule<ID, Attribute>>
    {
        typedef ID id;
        typedef Attribute attribute_type;
        static bool const has_attribute =
            !is_same<Attribute, unused_type>::value;

        template <typename RHS>
        rule_definition<
            ID, typename extension::as_parser<RHS>::value_type, Attribute>
        operator=(RHS const& rhs) const
        {
            typedef rule_definition<
                ID, typename extension::as_parser<RHS>::value_type, Attribute>
            result_type;

            return result_type(as_parser(rhs));
        }

        template <typename Iterator, typename Context, typename Attribute_>
        bool parse(Iterator& first, Iterator const& last
          , Context& context, Attribute_& attr) const
        {
            return get<ID>(context).rhs.parse(first, last, context, attr);
        }
    };
}}}

#endif
