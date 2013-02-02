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

#include <boost/spirit/home/x3/nonterminal/detail/rule.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/spirit/home/support/context.hpp>

#if !defined(BOOST_SPIRIT_NO_RTTI)
#include <typeinfo>
#endif

namespace boost { namespace spirit { namespace x3
{
    template <typename RHS, typename Attribute>
    struct rule_definition_context
    {
        RHS const& rhs;
        Attribute* attr_ptr;
    };

    template <typename ID, typename RHS, typename Attribute>
    struct rule_definition : parser<rule_definition<ID, RHS, Attribute>>
    {
        typedef rule_definition<ID, RHS, Attribute> this_type;
        typedef ID id;
        typedef RHS rhs_type;
        typedef Attribute attribute_type;
        static bool const has_attribute =
            !is_same<Attribute, unused_type>::value;
        static bool const handles_container =
            traits::is_container<Attribute>::value;

        rule_definition(RHS rhs, char const* name)
          : rhs(rhs), name(name) {}

        template <typename Iterator, typename Context, typename Attribute_>
        bool parse(Iterator& first, Iterator const& last
          , Context& context, Attribute_& attr) const
        {
            typedef
                rule_definition_context<rhs_type, attribute_type>
            r_context_type;
            r_context_type r_context = { rhs, 0 };

            typedef
                spirit::context<ID, r_context_type, Context>
            our_context_type;
            our_context_type our_context(r_context,  context);

            return detail::parse_rule<attribute_type, ID>::call(
                name, first, last, our_context, attr);
        }

        RHS rhs;
        char const* name;
    };

    template <typename ID, typename Attribute = unused_type>
    struct rule : parser<rule<ID, Attribute>>
    {
        typedef ID id;
        typedef Attribute attribute_type;
        static bool const has_attribute =
            !is_same<Attribute, unused_type>::value;
        static bool const handles_container =
            traits::is_container<Attribute>::value;

#if !defined(BOOST_SPIRIT_NO_RTTI)
        rule(char const* name = typeid(rule).name()) : name(name) {}
#else
        rule(char const* name = "unnamed") : name(name) {}
#endif

        template <typename RHS>
        rule_definition<
            ID, typename extension::as_parser<RHS>::value_type, Attribute>
        operator=(RHS const& rhs) const
        {
            typedef rule_definition<
                ID, typename extension::as_parser<RHS>::value_type, Attribute>
            result_type;

            return result_type(as_parser(rhs), name);
        }

        template <typename Iterator, typename Context, typename Attribute_>
        bool parse(Iterator& first, Iterator const& last
          , Context& context, Attribute_& attr) const
        {
            return detail::parse_rule<attribute_type, ID>::call(
                name, first, last, context, attr);
        }

        char const* name;
    };

    template <typename ID, typename Attribute>
    struct get_info<rule<ID, Attribute>>
    {
        typedef std::string result_type;
        std::string operator()(rule<ID, Attribute> const& p) const
        {
            return p.name;
        }
    };

    template <typename ID, typename Attribute, typename RHS>
    struct get_info<rule_definition<ID, RHS, Attribute>>
    {
        typedef std::string result_type;
        std::string operator()(rule_definition<ID, RHS, Attribute> const& p) const
        {
            return p.name;
        }
    };
}}}

#endif
