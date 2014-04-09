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
#include <boost/spirit/home/x3/nonterminal/detail/transform_attribute.hpp>
#include <boost/type_traits/is_class.hpp>
#include <boost/utility/enable_if.hpp>

namespace boost { namespace spirit { namespace x3
{
    struct rule_context_tag;

    namespace detail
    {
        // Detect if F accepts only the context and nothing else.
        // Using technique presented in http://tinyurl.com/ktscp3
        // by Eric Niebler (Boost Proto)

        struct uncallable
        {
            uncallable const &operator,(int) const;
        };

        template <typename T>
        type_traits::no_type is_uncallable(T const&);

        type_traits::yes_type is_uncallable(uncallable const&);

        template <typename F, typename Arg>
        struct is_unary_impl
        {
            template <typename F2>
            struct fwrap : F2
            {
                fwrap();
                typedef uncallable const& (*fptr)(unused_type);
                operator fptr() const;
            };

            static fwrap<F>& fun;
            static Arg& arg;

            static bool const value = (
                sizeof(type_traits::no_type) == sizeof(is_uncallable( (fun(arg), 0) ))
            );

            typedef mpl::bool_<value> type;
        };

        template <typename F, typename A, typename Enable = void>
        struct is_unary : mpl::false_ {};

        template <typename F, typename A>
        struct is_unary<F, A, typename enable_if<is_class<F>>::type>
            : is_unary_impl<F, A>::type {};

        template <typename RT, typename A, typename T>
        struct is_unary<RT(*)(T), A> : is_convertible<A, T> {};

        template <typename F>
        struct is_nullary_impl
        {
            template <typename F2>
            struct fwrap : F2
            {
                fwrap();
                typedef uncallable const& (*fptr)(...);
                operator fptr() const;
            };

            static fwrap<F>& fun;

            static bool const value = (
                sizeof(type_traits::no_type) == sizeof(is_uncallable( (fun(), 0) ))
            );

            typedef mpl::bool_<value> type;
        };

        template <typename F, typename Enable = void>
        struct is_nullary : mpl::false_ {};

        template <typename F>
        struct is_nullary<F, typename enable_if<is_class<F>>::type>
            : is_nullary_impl<F>::type {};

        template <typename RT>
        struct is_nullary<RT(*)()> : mpl::true_ {};
    }

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
        static bool const is_pass_through_unary = true;
        static bool const has_action = true;

        action(Subject const& subject, Action f)
          : base_type(subject), f(f) {}

        template <typename Context, typename Attribute>
        bool call_action(Context const& context, Attribute& attr, mpl::false_) const
        {
            bool pass = true;
            auto action_context = make_context<parse_pass_context_tag>(pass, context);
            f(action_context, attr); // pass in the context and attribute
            return pass;
        }

        template <typename Context, typename Attribute>
        bool call_action(Context const& context, Attribute& attr, mpl::true_) const
        {
            f(attr); // pass attribute only
            return true;
        }

        // action wants attribute
        template <typename Iterator, typename Context, typename Attribute>
        bool parse(Iterator& first, Iterator const& last
          , Context const& context, Attribute& attr, mpl::false_) const
        {
            Iterator save = first;
            if (this->subject.parse(first, last, context, attr))
            {
                typename detail::is_unary<Action, Attribute> is_unary;
                if (call_action(context, attr, is_unary))
                    return true;

                // reset iterators if semantic action failed the match
                // retrospectively
                first = save;
            }
            return false;
        }

        // attr==unused, action wants attribute
        template <typename Iterator, typename Context>
        bool parse(Iterator& first, Iterator const& last
          , Context const& context, unused_type, mpl::false_) const
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
            return parse(first, last, context, attr, mpl::false_());
        }

        // action does not want context and attribute
        template <typename Iterator, typename Context, typename Attribute>
        bool parse(Iterator& first, Iterator const& last
          , Context const& context, Attribute& attr, mpl::true_) const
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
            typename detail::is_nullary<Action> is_nullary;
            return parse(first, last, context, attr, is_nullary);
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
