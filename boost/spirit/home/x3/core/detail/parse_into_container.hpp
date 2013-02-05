/*=============================================================================
    Copyright (c) 2001-2013 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(SPIRIT_PARSE_INTO_CONTAINER_JAN_15_2013_0957PM)
#define SPIRIT_PARSE_INTO_CONTAINER_JAN_15_2013_0957PM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/support/traits/container_traits.hpp>
#include <boost/spirit/home/support/traits/attribute_of.hpp>
#include <boost/spirit/home/support/traits/is_substitute.hpp>
#include <boost/mpl/and.hpp>
#include <boost/fusion/include/front.hpp>

namespace boost { namespace spirit { namespace x3 { namespace detail
{
    template <typename Parser>
    struct parse_into_container_base_impl
    {
        // Parser has attribute (synthesize; Attribute is a container)
        template <typename Iterator, typename Context, typename Attribute>
        static bool call_synthesize(
            Parser const& parser
          , Iterator& first, Iterator const& last
          , Context const& context, Attribute& attr, mpl::false_)
        {
            // synthesized attribute needs to be default constructed
            typedef typename
                traits::container_value<Attribute>::type
            value_type;
            value_type val = value_type();

            if (!parser.parse(first, last, context, val))
                return false;

            // push the parsed value into our attribute
            traits::push_back(attr, val);
            return true;
        }

        // Parser has attribute (synthesize; Attribute is a single element fusion sequence)
        template <typename Iterator, typename Context, typename Attribute>
        static bool call_synthesize(
            Parser const& parser
          , Iterator& first, Iterator const& last
          , Context const& context, Attribute& attr, mpl::true_)
        {
            static_assert(traits::has_size<Attribute, 1>::value,
                "Expecting a single element fusion sequence");
            return call_synthesize(parser, first, last, context,
                fusion::front(attr), mpl::false_());
        }

        // Parser has attribute (synthesize)
        template <typename Iterator, typename Context, typename Attribute>
        static bool call(
            Parser const& parser
          , Iterator& first, Iterator const& last
          , Context const& context, Attribute& attr, mpl::true_)
        {
            return call_synthesize(parser, first, last, context, attr
              , fusion::traits::is_sequence<Attribute>());
        }

        // Parser has no attribute (pass unused)
        template <typename Iterator, typename Context, typename Attribute>
        static bool call(
            Parser const& parser
          , Iterator& first, Iterator const& last
          , Context const& context, Attribute& attr, mpl::false_)
        {
            return parser.parse(first, last, context, unused);
        }

        template <typename Iterator, typename Context, typename Attribute>
        static bool call(
            Parser const& parser
          , Iterator& first, Iterator const& last
          , Context const& context, Attribute& attr)
        {
            return call(parser, first, last, context, attr
              , mpl::bool_<Parser::has_attribute>());
        }
    };

    template <typename Parser, typename Enable = void>
    struct parse_into_container_impl : parse_into_container_base_impl<Parser> {};

    template <typename Parser, typename Container>
    struct parser_attr_is_substitute_for_container_value
        : traits::is_substitute<
            typename traits::attribute_of<Parser>::type
          , typename traits::container_value<Container>::type
        >
    {};

    template <typename Parser>
    struct parse_into_container_impl<Parser,
        typename enable_if_c<(Parser::handles_container)>::type>
    {
        template <typename Iterator, typename Context, typename Attribute>
        static bool call(
            Parser const& parser
          , Iterator& first, Iterator const& last
          , Context const& context, Attribute& attr, mpl::true_)
        {
            return parse_into_container_base_impl<Parser>::call(
                parser, first, last, context, attr);
        }

        template <typename Iterator, typename Context, typename Attribute>
        static bool call(
            Parser const& parser
          , Iterator& first, Iterator const& last
          , Context const& context, Attribute& attr, mpl::false_)
        {
            return parser.parse(first, last, context, attr);
        }

        template <typename Iterator, typename Context, typename Attribute>
        static bool call(
            Parser const& parser
          , Iterator& first, Iterator const& last
          , Context const& context, Attribute& attr)
        {
            return call(parser, first, last, context, attr,
                parser_attr_is_substitute_for_container_value<Parser, Attribute>());
        }
    };

    template <typename Parser, typename Iterator
      , typename Context, typename Attribute>
    bool parse_into_container(
        Parser const& parser
      , Iterator& first, Iterator const& last
      , Context const& context, Attribute& attr)
    {
        return parse_into_container_impl<Parser>::call(
            parser, first, last, context, attr);
    }

}}}}

#endif
