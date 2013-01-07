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
#include <boost/spirit/home/x3/core/detail/get_types.hpp>
#include <boost/variant/variant.hpp>

#include <boost/mpl/vector.hpp>
#include <boost/mpl/push_back.hpp>
#include <boost/mpl/push_front.hpp>
#include <boost/mpl/joint_view.hpp>
#include <boost/mpl/copy_if.hpp>
#include <boost/mpl/not.hpp>
#include <boost/mpl/if.hpp>

namespace boost { namespace spirit { namespace x3
{
    template <typename Left, typename Right>
    struct alternative;
}}}

namespace boost { namespace spirit { namespace x3 { namespace detail
{
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

    template <typename Parser, typename Attribute>
    struct pass_parser_attribute
    {
        typedef typename traits::attribute_of<Parser>::type type;

        static type
        call(Attribute&)
        {
            return type();
        }
    };

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

    template <typename T>
    struct get_alternative_base
    {
        typedef T type;
    };

    template <typename L, typename R>
    struct get_alternative_base<alternative<L, R>>
    {
        typedef binary_parser<L, R, alternative<L, R>> type;
    };

    template <typename L, typename R>
    struct get_alternative_types :
        get_types<
            typename get_alternative_base<L>::type
          , typename get_alternative_base<R>::type>
    {
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

        // Build a fusion::deque
        typedef typename
            make_variant_over<filtered_types>::type
        type;
    };

}}}}

#endif
