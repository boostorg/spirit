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
#include <boost/spirit/home/x3/nonterminal/detail/decompose_attribute.hpp>
#include <boost/spirit/home/x3/nonterminal/detail/check_args.hpp>
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

    template <typename Attribute, typename Params>
    struct rule_context
    {
        Attribute& val() const
        {
            BOOST_ASSERT(attr_ptr);
            return *attr_ptr;
        }

        Params& params() const
        {
            BOOST_ASSERT(params_ptr);
            return *params_ptr;
        }
        
        Attribute* attr_ptr = nullptr;
        Params* params_ptr = nullptr;
    };

    template <typename Context>
    inline auto
    _val(Context const& context)
    -> decltype(x3::get<rule_context_tag>(context).val())
    {
        return x3::get<rule_context_tag>(context).val();
    }
    
    template <typename Context>
    inline auto
    _params(Context const& context)
    -> decltype(x3::get<rule_context_tag>(context).params())
    {
        return x3::get<rule_context_tag>(context).params();
    }

    template <typename ID, typename RHS, typename Attribute, typename Params, bool explicit_attribute_propagation_>
    struct rule_definition : parser<rule_definition<ID, RHS, Attribute, Params, explicit_attribute_propagation_>>
    {
        typedef rule_definition<ID, RHS, Attribute, Params, explicit_attribute_propagation_> this_type;
        typedef ID id;
        typedef RHS rhs_type;
        typedef Attribute attribute_type;
        static bool const has_attribute =
            !is_same<Attribute, unused_type>::value;
        static bool const handles_container =
            traits::is_container<Attribute>::value;
        static bool const caller_is_pass_through_unary = true;
        static bool const explicit_attribute_propagation =
            explicit_attribute_propagation_;

        rule_definition(RHS rhs, char const* name)
          : rhs(rhs), name(name) {}

        template <typename Iterator, typename Context, typename Attribute_, typename... Ts>
        bool parse(Iterator& first, Iterator const& last
          , Context const& context, Attribute_& attr, Ts&&... ts) const
        {
            static_assert(
                detail::check_args<Params, Ts...>::value
              , "args/params not matched");
            
            rule_context<Attribute, Params> r_context;
            auto rule_ctx1 = make_context<rule_context_with_id_tag<ID>>(r_context, context);
            auto rule_ctx2 = make_context<rule_context_tag>(r_context, rule_ctx1);
            auto this_context = make_context<ID>(*this, rule_ctx2);
            
            return detail::parse_rule<attribute_type, Params, ID>
                ::call_rule_definition(
                    rhs, name, first, last, this_context
                  , attr, Params(std::forward<Ts>(ts)...)
                  , r_context.attr_ptr, r_context.params_ptr
                  , mpl::bool_<explicit_attribute_propagation>());
        }

        RHS rhs;
        char const* name;
    };

    template <typename ID, typename Attribute = unused_type>
    struct rule : parser<rule<ID, Attribute>>
    {
        typedef ID id;
        typedef detail::decompose_attribute<Attribute> decompose_attr;
        typedef typename decompose_attr::result_type attribute_type;
        typedef typename decompose_attr::params_type params_type;
        static bool const has_attribute =
            !is_same<attribute_type, unused_type>::value;
        static bool const handles_container =
            traits::is_container<attribute_type>::value;
        static bool const caller_is_pass_through_unary = true;

#if !defined(BOOST_SPIRIT_X3_NO_RTTI)
        rule(char const* name = typeid(rule).name()) : name(name) {}
#else
        rule(char const* name = "unnamed") : name(name) {}
#endif

        template <typename RHS>
        rule_definition<
            ID, typename extension::as_parser<RHS>::value_type, attribute_type, params_type, false>
        operator=(RHS const& rhs) const
        {
            return {as_parser(rhs), name};
        }

        template <typename RHS>
        rule_definition<
            ID, typename extension::as_parser<RHS>::value_type, attribute_type, params_type, true>
        operator%=(RHS const& rhs) const
        {
            return {as_parser(rhs), name};
        }

        template <typename Iterator, typename Context, typename Attribute_, typename... Ts>
        bool parse(Iterator& first, Iterator const& last
          , Context const& context, Attribute_& attr, Ts&&... ts) const
        {
            return detail::parse_rule<attribute_type, params_type, ID>::call_from_rule(
                x3::get<ID>(context), name
              , first, last, context, attr
              , x3::get<rule_context_with_id_tag<ID>>(context)
              , std::forward<Ts>(ts)...);
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

    template <typename ID, typename Attribute, typename Params, typename RHS, bool explicit_attribute_propagation>
    struct get_info<rule_definition<ID, RHS, Attribute, Params, explicit_attribute_propagation>>
    {
        typedef std::string result_type;
        std::string operator()(rule_definition<ID, RHS, Attribute, Params, explicit_attribute_propagation> const& p) const
        {
            return p.name;
        }
    };
}}}

#endif
