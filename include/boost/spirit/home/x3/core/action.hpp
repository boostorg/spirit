/*=============================================================================
    Copyright (arg) 2001-2013 Joel de Guzman

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
#include <boost/spirit/home/x3/support/utility/type_traits.hpp>
#include <boost/spirit/home/x3/nonterminal/detail/transform_attribute.hpp>
#include <boost/type_traits/is_class.hpp>
#include <boost/type_traits/is_member_pointer.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/mem_fn.hpp>


namespace boost { namespace spirit { namespace x3 { namespace detail
{
    template <typename Action, typename Context, typename Attribute>
    using action_category = typename mpl::eval_if<
        is_callable<Action(Context, Attribute)>, mpl::int_<2>
      , mpl::eval_if<is_callable<Action(Attribute)>, mpl::int_<1>
          , mpl::if_<is_callable<Action()>, mpl::int_<0>, mpl::int_<-1>>>>::type;

    template <typename Action, typename = void>
    struct wrap_action
    {
        typedef Action type;

        static Action&& apply(Action& f)
        {
            return std::move(f);
        }
    };
    
    template <typename Action>
    struct wrap_action<Action,
        typename enable_if<is_member_pointer<Action>>::type>
    {
        typedef decltype(mem_fn(declval<Action>())) type;

        static type apply(Action f)
        {
            return mem_fn(f);
        }
    };
}}}};

namespace boost { namespace spirit { namespace x3
{
    struct parse_pass_context_tag;
    
    template <typename Context>
    inline bool& _pass(Context const& context)
    {
        return x3::get<parse_pass_context_tag>(context);
    }

    template <typename Subject, typename Action>
    struct action : unary_parser<Subject, action<Subject, Action>>
    {
        typedef unary_parser<Subject, action<Subject, Action>> base_type;
        typedef detail::wrap_action<Action> wrap_action;
        typedef typename wrap_action::type action_type;
        static bool const is_pass_through_unary = true;
        static bool const has_action = true;

        action(Subject const& subject, Action f)
          : base_type(subject), f(wrap_action::apply(f)) {}

        template <typename Context, typename Attribute>
        bool call_action(mpl::int_<2>, Context const& context, Attribute& attr) const
        {
            bool pass = true;
            auto action_context = make_context<parse_pass_context_tag>(pass, context);
            f(action_context, attr); // pass in the context and attribute
            return pass;
        }

        template <typename Context, typename Attribute>
        bool call_action(mpl::int_<1>, Context const& context, Attribute& attr) const
        {
            f(attr); // pass attribute only
            return true;
        }

        // action wants attribute
        template <int N, typename Iterator, typename Context, typename Attribute>
        typename enable_if_c<(N > 0), bool>::type
        parse_impl(mpl::int_<N> tag, Iterator& first, Iterator const& last
          , Context const& context, Attribute& attr) const
        {
            Iterator save = first;
            if (this->subject.parse(first, last, context, attr))
            {
                if (call_action(tag, context, attr))
                    return true;

                // reset iterators if semantic action failed the match
                // retrospectively
                first = save;
            }
            return false;
        }

        // attr==unused, action wants attribute
        template <int N, typename Iterator, typename Context>
        typename enable_if_c<(N > 0), bool>::type
        parse_impl(mpl::int_<N> tag, Iterator& first, Iterator const& last
          , Context const& context, unused_type) const
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
            return parse_impl(tag, first, last, context, attr);
        }

        // action does not want context and attribute
        template <typename Iterator, typename Context, typename Attribute>
        bool parse_impl(mpl::int_<0>, Iterator& first, Iterator const& last
          , Context const& context, Attribute& attr) const
        {
            Iterator save = first;
            if (this->subject.parse(first, last, context, attr))
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
        template <typename Iterator, typename Context, typename Attribute>
        bool parse(Iterator& first, Iterator const& last
          , Context const& context, Attribute& attr) const
        {
            using tag = detail::action_category<
                action_type const
              , x3::context<parse_pass_context_tag, bool, Context>
              , typename traits::attribute_of<Subject, Context>::type>;

            static_assert(tag::value != -1, "invalid action");
            return parse_impl(tag(), first, last, context, attr);
        }

        action_type f;
    };

    template <typename P, typename Action>
    inline action<typename extension::as_parser<P>::value_type, Action>
    operator/(P const& p, Action f)
    {
        return {as_parser(p), f};
    }
}}}

#endif
