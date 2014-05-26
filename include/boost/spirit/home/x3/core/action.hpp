/*=============================================================================
    Copyright (arg) 2001-2014 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(SPIRIT_ACTION_JANUARY_07_2007_1128AM)
#define SPIRIT_ACTION_JANUARY_07_2007_1128AM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/x3/support/context.hpp>
#include <boost/spirit/home/x3/support/traits/attribute_of.hpp>
#include <boost/spirit/home/x3/support/traits/make_attribute.hpp>
#include <boost/spirit/home/x3/support/utility/is_callable.hpp>
#include <boost/spirit/home/x3/nonterminal/detail/transform_attribute.hpp>
#include <boost/type_traits/is_class.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/utility/enable_if.hpp>

namespace boost { namespace spirit { namespace x3
{
    struct raw_attribute_type;
    struct rule_context_tag;
    struct parse_pass_context_tag;

    template <typename Context>
    inline bool& _pass(Context const& context)
    {
        return x3::get<parse_pass_context_tag>(context);
    }
    
    struct rule_val_context_tag;
    template <typename Context>

    inline auto _val(Context const& context)
        -> decltype(x3::get<rule_val_context_tag>(context))
    {
        return x3::get<rule_val_context_tag>(context);
    }

    template <typename Subject, typename Action>
    struct action : unary_parser<Subject, action<Subject, Action>>
    {
        typedef unary_parser<Subject, action<Subject, Action>> base_type;
        static bool const is_pass_through_unary = true;
        static bool const has_action = true;

        action(Subject const& subject, Action f)
          : base_type(subject), f(f) {}

        template <typename Context, typename RuleContext, typename Attribute>
        bool call_action(Context const& context, RuleContext& rcontext, Attribute& attr, mpl::false_) const
        {
            bool pass = true;
            auto action_context = make_context<parse_pass_context_tag>(pass, context);
            auto val_context = make_context<rule_val_context_tag>(rcontext, action_context);
            f(val_context, attr); // pass in the context and attribute
            return pass;
        }

        template <typename Context, typename RuleContext, typename Attribute>
        bool call_action(Context const&, RuleContext&, Attribute& attr, mpl::true_) const
        {
            f(attr); // pass attribute only
            return true;
        }

        // action wants attribute
        template <typename Iterator, typename Context
          , typename RuleContext, typename Attribute>
        bool parse(Iterator& first, Iterator const& last
          , Context const& context, RuleContext& rcontext, Attribute& attr, mpl::false_) const
        {
            Iterator save = first;
            if (this->subject.parse(first, last, context, rcontext, attr))
            {
                if (call_action(context, rcontext, attr, is_callable<Action(Attribute&)>()))
                    return true;

                // reset iterators if semantic action failed the match
                // retrospectively
                first = save;
            }
            return false;
        }
        
        // attr==raw_attribute_type, action wants iterator_range (see raw.hpp)
        template <typename Iterator, typename Context, typename RuleContext>
        bool parse(Iterator& first, Iterator const& last
          , Context const& context, RuleContext& rcontext, raw_attribute_type&, mpl::false_) const
        {
            boost::iterator_range<Iterator> rng;
            // synthesize the attribute since one is not supplied
            return parse(first, last, context, rcontext, rng, mpl::false_());
        }

        // attr==unused, action wants attribute
        template <typename Iterator, typename Context, typename RuleContext>
        bool parse(Iterator& first, Iterator const& last
          , Context const& context, RuleContext& rcontext, unused_type, mpl::false_) const
        {
            typedef typename
                traits::attribute_of<action<Subject, Action>, Context>::type
            attribute_type;
            typedef traits::make_attribute<attribute_type, unused_type> make_attribute;
            typedef traits::transform_attribute<
                typename make_attribute::type, attribute_type, parser_id>
            transform;

            // synthesize the attribute since one is not supplied
            typename make_attribute::type made_attr = make_attribute::call(unused_type());
            typename transform::type attr = transform::pre(made_attr);
            return parse(first, last, context, rcontext, attr, mpl::false_());
        }

        // action does not want context and attribute
        template <typename Iterator, typename Context
            , typename RuleContext, typename Attribute>
        bool parse(Iterator& first, Iterator const& last
          , Context const& context, RuleContext& rcontext, Attribute& attr, mpl::true_) const
        {
            Iterator save = first;
            if (this->subject.parse(first, last, context, rcontext, attr))
            {
                f();
                return true;

                // reset iterators if semantic action failed the match
                // retrospectively
                first = save;
            }
            return false;
        }

        // main parse function
        template <typename Iterator, typename Context
            , typename RuleContext, typename Attribute>
        bool parse(Iterator& first, Iterator const& last
          , Context const& context, RuleContext& rcontext, Attribute& attr) const
        {
            return parse(first, last, context, rcontext, attr, is_callable<Action()>());
        }

        Action f;
    };

    template <typename P, typename Action>
    inline action<typename extension::as_parser<P>::value_type, Action>
    operator/(P const& p, Action f)
    {
        return {as_parser(p), f};
    }
}}}

#endif
