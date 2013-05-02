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
#include <boost/utility/addressof.hpp>

#if defined(BOOST_SPIRIT_DEBUG)
#include <boost/spirit/home/x3/nonterminal/simple_trace.hpp>
#endif

namespace boost { namespace spirit { namespace x3
{
    template <typename ID>
    struct rule_context_with_id_tag;
}}}

namespace boost { namespace spirit { namespace x3 { namespace detail
{
#if defined(BOOST_SPIRIT_DEBUG)
    template <typename Iterator, typename Attribute>
    struct context_debug
    {
        context_debug(
            char const* rule_name
          , Iterator const& first, Iterator const& last
          , Attribute const& attr)
          : fail(true), rule_name(rule_name)
          , first(first), last(last)
          , attr(attr)
          , f(detail::get_simple_trace())
        {
            f(first, last, attr, pre_parse, rule_name);
        }

        ~context_debug()
        {
            auto status = fail ? failed_parse : successful_parse;
            f(first, last, attr, status, rule_name);
        }

        bool fail;
        char const* rule_name;
        Iterator const& first;
        Iterator const& last;
        Attribute const& attr;
        detail::simple_trace_type& f;
    };
#endif

    template <typename Attribute>
    struct attr_pointer_scope
    {
        attr_pointer_scope(Attribute*& ptr, Attribute* set)
          : save(ptr), ptr(ptr) { ptr = set; }
        ~attr_pointer_scope() { ptr = save; }

        Attribute* save;
        Attribute*& ptr;
    };

    template <>
    struct attr_pointer_scope<unused_type>
    {
        attr_pointer_scope(unused_type, unused_type) {}
        ~attr_pointer_scope() {}
    };

    template <typename Attribute, typename ID>
    struct parse_rule
    {
        template <typename RHS, typename Iterator, typename Context, typename ActualAttribute>
        static bool parse_rhs(
            RHS const& rhs
          , Iterator& first, Iterator const& last
          , Context const& context, ActualAttribute& attr
          , mpl::false_)
        {
            return rhs.parse(first, last, context, attr);
        }

        template <typename RHS, typename Iterator, typename Context, typename ActualAttribute>
        static bool parse_rhs(
            RHS const& rhs
          , Iterator& first, Iterator const& last
          , Context const& context, ActualAttribute& attr
          , mpl::true_)
        {
            return rhs.parse(first, last, context, unused);
        }

        template <typename RHS, typename Iterator, typename Context, typename ActualAttribute>
        static bool parse_rhs(
            RHS const& rhs
          , Iterator& first, Iterator const& last
          , Context const& context, ActualAttribute& attr)
        {
            return parse_rhs(rhs, first, last, context, attr
              , mpl::bool_<(RHS::has_action)>());
        }

        template <typename RHS, typename Iterator, typename Context
            , typename ActualAttribute, typename AttributePtr>
        static bool call_rule_definition(
            RHS const& rhs
          , char const* rule_name
          , Iterator& first, Iterator const& last
          , Context const& context, ActualAttribute& attr, AttributePtr*& attr_ptr)
        {
            typedef traits::make_attribute<Attribute, ActualAttribute> make_attribute;

            // do down-stream transformation, provides attribute for
            // rhs parser
            typedef traits::transform_attribute<
                typename make_attribute::type, Attribute, parser_id>
            transform;

            typedef typename make_attribute::value_type value_type;
            typedef typename transform::type transform_attr;
            value_type made_attr = make_attribute::call(attr);
            transform_attr attr_ = transform::pre(made_attr);

#if defined(BOOST_SPIRIT_DEBUG)
            context_debug<Iterator, typename make_attribute::value_type>
                dbg(rule_name, first, last, made_attr);
#endif
            attr_pointer_scope<typename remove_reference<transform_attr>::type>
                attr_scope(attr_ptr, boost::addressof(attr_));
            if (parse_rhs(rhs, first, last, context, attr_))
            {
                // do up-stream transformation, this integrates the results
                // back into the original attribute value, if appropriate
                traits::post_transform(attr, attr_);

#if defined(BOOST_SPIRIT_DEBUG)
                dbg.fail = false;
#endif
                return true;
            }
            return false;
        }

        template <typename Rule, typename Iterator, typename Context
            , typename ActualAttribute, typename AttributeContext>
        static bool call_from_rule(
            Rule const& rule
          , char const* rule_name
          , Iterator& first, Iterator const& last
          , Context const& context, ActualAttribute& attr, AttributeContext& attr_ctx)
        {
            // This is called when a rule-body has already been established.
            // The rule body is already established by the rule_definition class,
            // we will not do it again. We'll simply call the RHS by calling
            // call_rule_definition.

            return call_rule_definition(
                rule.rhs, rule_name, first, last, context, attr, attr_ctx.attr_ptr);
        }

        template <typename Rule, typename Iterator, typename Context
            , typename ActualAttribute>
        static bool call_from_rule(
            Rule const& rule
          , char const* rule_name
          , Iterator& first, Iterator const& last
          , Context const& context, ActualAttribute& attr, unused_type)
        {
            // This is called when a rule-body has *not yet* been established.
            // The rule body is established by the rule_definition class, so
            // we call it to parse and establish the rule-body.

            return rule.parse(first, last, context, attr);
        }
    };
}}}}

#endif
