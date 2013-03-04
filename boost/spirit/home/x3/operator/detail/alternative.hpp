/*=============================================================================
    Copyright (c) 2001-2013 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(SPIRIT_ALTERNATIVE_DETAIL_JAN_07_2013_1245PM)
#define SPIRIT_ALTERNATIVE_DETAIL_JAN_07_2013_1245PM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/support/traits/attribute_of.hpp>
#include <boost/spirit/home/support/traits/is_substitute.hpp>
#include <boost/spirit/home/support/traits/is_variant.hpp>
#include <boost/spirit/home/support/traits/tuple_traits.hpp>
#include <boost/spirit/home/support/traits/move_to.hpp>
#include <boost/spirit/home/x3/core/detail/parse_into_container.hpp>
#include <boost/variant/variant.hpp>

#include <boost/mpl/copy_if.hpp>
#include <boost/mpl/not.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/joint_view.hpp>

#include <boost/fusion/include/front.hpp>

namespace boost { namespace spirit { namespace x3
{
    template <typename Left, typename Right>
    struct alternative;
}}}

namespace boost { namespace spirit { namespace x3 { namespace detail
{
    template <typename Variant, typename Expected>
    struct has_substitute_impl
    {
        // Find the type from the variant that can be a substitute for Expected.
        // return true_ if one is found, else false_

        typedef Variant variant_type;
        typedef typename variant_type::types types;
        typedef typename mpl::end<types>::type end;

        typedef typename
            mpl::find_if<types, is_same<mpl::_1, Expected> >::type
        iter_1;

        typedef typename
            mpl::eval_if<
                is_same<iter_1, end>,
                mpl::find_if<types, traits::is_substitute<mpl::_1, Expected> >,
                mpl::identity<iter_1>
            >::type
        iter;

        typedef mpl::not_<is_same<iter, end>> type;
    };

    template <typename Variant, typename Expected>
    struct has_substitute : has_substitute_impl<Variant, Expected>::type {};

    template <typename Expected>
    struct has_substitute<unused_type, Expected> : mpl::true_ {};

    template <typename Expected>
    struct has_substitute<unused_type const, Expected> : mpl::true_ {};

    template <typename Variant, typename Expected>
    struct find_substitute
    {
        // Get the type from the variant that can be a substitute for Expected.
        // If none is found, just return Expected

        typedef Variant variant_type;
        typedef typename variant_type::types types;
        typedef typename mpl::end<types>::type end;

        typedef typename
            mpl::find_if<types, is_same<mpl::_1, Expected> >::type
        iter_1;

        typedef typename
            mpl::eval_if<
                is_same<iter_1, end>,
                mpl::find_if<types, traits::is_substitute<mpl::_1, Expected> >,
                mpl::identity<iter_1>
            >::type
        iter;

        typedef typename
            mpl::eval_if<
                is_same<iter, end>,
                mpl::identity<Expected>,
                mpl::deref<iter>
            >::type
        type;
    };

    struct pass_variant_unused
    {
        typedef unused_type type;

        template <typename T>
        static unused_type
        call(T&)
        {
            return unused_type();
        }
    };

    template <typename Attribute>
    struct pass_variant_used
    {
        typedef Attribute& type;

        static Attribute&
        call(Attribute& v)
        {
            return v;
        }
    };

    template <>
    struct pass_variant_used<unused_type> : pass_variant_unused {};

    template <typename Parser, typename Attribute, typename Enable = void>
    struct pass_parser_attribute
    {
        typedef typename traits::attribute_of<Parser>::type expected_type;
        typedef typename
            find_substitute<Attribute, expected_type>::type
        substitute_type;

        typedef typename
            mpl::if_<
                is_same<Attribute, substitute_type>
              , Attribute&
              , substitute_type
            >::type
        type;

        template <typename Attribute_>
        static Attribute_&
        call(Attribute_& attr, mpl::true_)
        {
            return attr;
        }

        template <typename Attribute_>
        static type
        call(Attribute_& attr, mpl::false_)
        {
            return type();
        }

        template <typename Attribute_>
        static type
        call(Attribute_& attr)
        {
            return call(attr, is_same<Attribute_, type>());
        }
    };

    // Pass non-varinat attributes as-is
    template <typename Parser, typename Attribute, typename Enable = void>
    struct pass_non_variant_attribute
    {
        typedef Attribute& type;

        static Attribute&
        call(Attribute& attr)
        {
            return attr;
        }
    };

    // Unwrap single element sequences
    template <typename Parser, typename Attribute>
    struct pass_non_variant_attribute<Parser, Attribute,
        typename enable_if<traits::is_size_one_sequence<Attribute>>::type>
    {
        typedef typename remove_reference<
            typename fusion::result_of::front<Attribute>::type>::type
        attr_type;

        typedef pass_parser_attribute<Parser, attr_type> pass;
        typedef typename pass::type type;

        template <typename Attribute_>
        static type
        call(Attribute_& attr)
        {
            return pass::call(fusion::front(attr));
        }
    };

    template <typename Parser, typename Attribute>
    struct pass_parser_attribute<Parser, Attribute,
        typename enable_if_c<(!traits::is_variant<Attribute>::value)>::type>
        : pass_non_variant_attribute<Parser, Attribute>
    {};

    template <typename Parser>
    struct pass_parser_attribute<Parser, unused_type> : pass_variant_unused {};

    template <typename Parser, typename Attribute>
    struct pass_variant_attribute :
        mpl::if_c<Parser::has_attribute
          , pass_parser_attribute<Parser, Attribute>
          , pass_variant_unused>::type
    {
        static bool const is_alternative = false;
    };

    template <typename L, typename R, typename Attribute>
    struct pass_variant_attribute<alternative<L, R>, Attribute> :
        mpl::if_c<alternative<L, R>::has_attribute
          , pass_variant_used<Attribute>
          , pass_variant_unused>::type
    {
        static bool const is_alternative = true;
    };

    template <typename L, typename R>
    struct get_alternative_types
    {
        typedef
            mpl::vector<
                typename traits::attribute_of<L>::type
              , typename traits::attribute_of<R>::type
            >
        type;
    };

    template <typename LL, typename LR, typename R>
    struct get_alternative_types<alternative<LL, LR>, R>
    {
        typedef typename
            mpl::push_back<
                typename get_alternative_types<LL, LR>::type
              , typename traits::attribute_of<R>::type
            >::type
        type;
    };

    template <typename L, typename RL, typename RR>
    struct get_alternative_types<L, alternative<RL, RR>>
    {
        typedef typename
            mpl::push_front<
                typename get_alternative_types<RL, RR>::type
              , typename traits::attribute_of<L>::type
            >::type
        type;
    };

    template <typename LL, typename LR, typename RL, typename RR>
    struct get_alternative_types<alternative<LL, LR>, alternative<RL, RR>>
    {
        typedef
            mpl::joint_view<
                typename get_alternative_types<LL, LR>::type
              , typename get_alternative_types<RL, RR>::type
            >
        type;
    };

    template <typename L, typename R>
    struct attribute_of_alternative
    {
        // Get all alternative attribute types
        typedef typename get_alternative_types<L, R>::type all_types;

        // Filter all unused_types
        typedef typename
            mpl::copy_if<
                all_types
              , mpl::not_<is_same<mpl::_1, unused_type>>
              , mpl::back_inserter<mpl::vector<>>
            >::type
        filtered_types;

        // Build a variant if filtered_types is not empty,
        // else just return unused_type
        typedef typename
            mpl::eval_if<
                mpl::empty<filtered_types>
              , mpl::identity<unused_type>
              , make_variant_over<filtered_types>
            >::type
        type;
    };

    template <typename Parser, typename Iterator, typename Context, typename Attribute>
    bool parse_alternative(
        Parser const& p, Iterator& first, Iterator const& last
      , Context const& context, Attribute& attr)
    {
        typedef detail::pass_variant_attribute<Parser, Attribute> pass;

        typename pass::type attr_ = pass::call(attr);
        if (p.parse(first, last, context, attr_))
        {
            if (!pass::is_alternative)
                traits::move_to(attr_, attr);
            return true;
        }
        return false;
    }


    template <typename Left, typename Right>
    struct parse_into_container_impl<alternative<Left, Right>>
    {
        typedef alternative<Left, Right> parser_type;

        template <typename Iterator, typename Context, typename Attribute>
        static bool call(
            parser_type const& parser
          , Iterator& first, Iterator const& last
          , Context const& context, Attribute& attr, mpl::true_)
        {
            return parse_alternative(parser, first, last, context, attr);
        }

        template <typename Iterator, typename Context, typename Attribute>
        static bool call(
            parser_type const& parser
          , Iterator& first, Iterator const& last
          , Context const& context, Attribute& attr, mpl::false_)
        {
            return parse_into_container_base_impl<parser_type>::call(
                parser, first, last, context, attr);
        }

        template <typename Iterator, typename Context, typename Attribute>
        static bool call(
            parser_type const& parser
          , Iterator& first, Iterator const& last
          , Context const& context, Attribute& attr)
        {
            typedef typename
                traits::attribute_of<parser_type>::type
            attribute_type;

            return call(parser, first, last, context, attr
              , has_substitute<attribute_type, Attribute>());
        }
    };

}}}}

#endif
