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
#include <boost/mpl/has_xxx.hpp>

namespace boost { namespace spirit { namespace x3 { namespace detail
{
    template <typename Parser, typename Enable = void>
    struct parse_into_container_impl
    {
        template <typename Iterator, typename Context, typename Attribute>
        static bool call(
            Parser const& parser
          , Iterator& first, Iterator const& last
          , Context& context, Attribute& attr)
        {
            if (Parser::has_attribute)
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
            else
            {
                return parser.parse(first, last, context, unused);
            }
        }
    };

    BOOST_MPL_HAS_XXX_TRAIT_DEF(handles_container_attribute)

    template <typename Parser>
    struct parse_into_container_impl<Parser,
        typename enable_if<has_handles_container_attribute<Parser>>::type>
    {
        template <typename Iterator, typename Context, typename Attribute>
        static bool call(
            Parser const& parser
          , Iterator& first, Iterator const& last
          , Context& context, Attribute& attr)
        {
            return parser.parse(first, last, context, attr);
        }
    };

    template <typename Parser, typename Iterator
      , typename Context, typename Attribute>
    bool parse_into_container(
        Parser const& parser
      , Iterator& first, Iterator const& last
      , Context& context, Attribute& attr)
    {
        return parse_into_container_impl<Parser>::call(
            parser, first, last, context, attr);
    }

}}}}

#endif
