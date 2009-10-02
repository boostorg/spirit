/*=============================================================================
    Copyright (c) 2001-2009 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_SPIRIT_PARSER_BINDER_DECEMBER_05_2008_0516_PM)
#define BOOST_SPIRIT_PARSER_BINDER_DECEMBER_05_2008_0516_PM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/fusion/include/at.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/spirit/home/support/has_semantic_action.hpp>

namespace boost { namespace spirit { namespace qi { namespace detail
{
    // parser_binder for plain rules
    template <typename Parser, typename Auto>
    struct parser_binder
    {

        parser_binder(Parser const& p)
          : p(p) {}

        template <typename Iterator, typename Skipper, typename Context>
        bool call(Iterator& first, Iterator const& last
          , Context& context, Skipper const& skipper, mpl::true_) const
        {
            // If DeducedAuto is false (semantic actions is present), the 
            // component's attribute is unused.
            return p.parse(first, last, context, skipper, unused);
        }

        template <typename Iterator, typename Skipper, typename Context>
        bool call(Iterator& first, Iterator const& last
          , Context& context, Skipper const& skipper, mpl::false_) const
        {
//             typedef typename traits::attribute_of<Parser>::type 
//                 subject_attribute_type;
//             typedef typename Context::attributes_type context_attributes_type;
//             typedef typename fusion::result_of::at_c<
//                 context_attributes_type, 0>::type attribute_type;
// 
//             // do down-stream transformation, provides attribute for embedded
//             // parser
//             typename traits::result_of::pre_transform<
//                 subject_attribute_type, attribute_type>::type 
//             attr_ = traits::pre_transform<subject_attribute_type>(
//                         fusion::at_c<0>(context.attributes));
// 
//             // If DeducedAuto is true (no semantic action), we pass the rule's 
//             // attribute on to the component.
//             if (!p.parse(first, last, context, skipper, attr_))
//                 return false;
// 
//             // do up-stream transformation, this mainly integrates the results
//             // back into the original attribute value, if appropriate
//             traits::post_transform<subject_attribute_type>(attr_
//               , fusion::at_c<0>(context.attributes));

            // If DeducedAuto is true (no semantic action), we pass the rule's 
            // attribute on to the component.
            return p.parse(first, last, context, skipper
                , fusion::at_c<0>(context.attributes));
        }

        template <typename Iterator, typename Skipper, typename Context>
        bool operator()(
            Iterator& first, Iterator const& last
          , Context& context, Skipper const& skipper) const
        {
            // If Auto is false, we need to deduce whether to apply auto rule
            typedef typename traits::has_semantic_action<Parser>::type auto_rule;
            return call(first, last, context, skipper, auto_rule());
        }

        Parser p;
    };

    // parser_binder for auto rules
    template <typename Parser>
    struct parser_binder<Parser, mpl::true_>
    {
        parser_binder(Parser const& p)
          : p(p) {}

        template <typename Iterator, typename Skipper, typename Context>
        bool operator()(
            Iterator& first, Iterator const& last
          , Context& context, Skipper const& skipper) const
        {
            // If Auto is true, we pass the rule's attribute on to the component.
            return p.parse(first, last, context, skipper
                , fusion::at_c<0>(context.attributes));
        }

        Parser p;
    };

    template <typename Auto, typename Parser>
    inline parser_binder<Parser, Auto>
    bind_parser(Parser const& p)
    {
        return parser_binder<Parser, Auto>(p);
    }
}}}}

#endif
