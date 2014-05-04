/*=============================================================================
    Copyright (c) 2001-2013 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_SPIRIT_X3_RULE_JAN_08_2012_0326PM)
#define BOOST_SPIRIT_X3_RULE_JAN_08_2012_0326PM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/x3/nonterminal/detail/rule.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/spirit/home/x3/support/context.hpp>

#if !defined(BOOST_SPIRIT_X3_NO_RTTI)
#include <typeinfo>
#endif

namespace boost { namespace spirit { namespace x3
{
    template <typename ID>
    struct identity {};

    struct rule_context_tag;

    template <typename ID>
    struct rule_context_with_id_tag;

    template <typename Attribute>
    struct rule_context
    {
        Attribute& val() const
        {
            BOOST_ASSERT(attr_ptr);
            return *attr_ptr;
        }

        Attribute* attr_ptr;
    };

    template <typename Context>
    inline auto
    _val(Context const& context)
    -> decltype(x3::get<rule_context_tag>(context).val())
    {
        return x3::get<rule_context_tag>(context).val();
    }

    template <typename ID, typename RHS, typename Attribute, bool explicit_attribute_propagation_>
    struct rule_definition : parser<rule_definition<ID, RHS, Attribute, explicit_attribute_propagation_>>
    {
        typedef rule_definition<ID, RHS, Attribute, explicit_attribute_propagation_> this_type;
        typedef ID id;
        typedef RHS rhs_type;
        typedef Attribute attribute_type;
        static bool const has_attribute =
            !is_same<Attribute, unused_type>::value;
        static bool const handles_container =
            traits::is_container<Attribute>::value;
        static bool const explicit_attribute_propagation =
            explicit_attribute_propagation_;

        rule_definition(RHS rhs, char const* name)
          : rhs(rhs), name(name) {}

        template <typename Iterator, typename Context, typename Attribute_>
        bool parse(Iterator& first, Iterator const& last
          , Context const& context, Attribute_& attr) const
        {
            rule_context<Attribute> r_context = { 0 };

            auto rule_ctx1 = make_context<rule_context_with_id_tag<ID>>(r_context, context);
            auto rule_ctx2 = make_context<rule_context_tag>(r_context, rule_ctx1);
            auto this_context = make_context<ID>(*this, rule_ctx2);

            return detail::parse_rule<attribute_type, ID>
                ::call_rule_definition(
                    rhs, name, first, last, this_context
                  , attr, r_context.attr_ptr
                  , mpl::bool_<explicit_attribute_propagation>());
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

#if !defined(BOOST_SPIRIT_X3_NO_RTTI)
        rule(char const* name = typeid(rule).name()) : name(name) {}
#else
        rule(char const* name = "unnamed") : name(name) {}
#endif

        template <typename RHS>
        rule_definition<
            ID, typename extension::as_parser<RHS>::value_type, Attribute, false>
        operator=(RHS const& rhs) const
        {
            return {as_parser(rhs), name};
        }

        template <typename RHS>
        rule_definition<
            ID, typename extension::as_parser<RHS>::value_type, Attribute, true>
        operator%=(RHS const& rhs) const
        {
            return {as_parser(rhs), name};
        }


        template <typename Iterator, typename Context, typename Attribute_>
        bool parse(Iterator& first, Iterator const& last
          , Context const& context, Attribute_& attr) const
        {
            return detail::parse_rule<attribute_type, ID>::call_from_rule(
                x3::get<ID>(context), name
              , first, last, context, attr
              , x3::get<rule_context_with_id_tag<ID>>(context));
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

    template <typename ID, typename Attribute, typename RHS, bool explicit_attribute_propagation>
    struct get_info<rule_definition<ID, RHS, Attribute, explicit_attribute_propagation>>
    {
        typedef std::string result_type;
        std::string operator()(rule_definition<ID, RHS, Attribute, explicit_attribute_propagation> const& p) const
        {
            return p.name;
        }
    };
}}}

#endif
