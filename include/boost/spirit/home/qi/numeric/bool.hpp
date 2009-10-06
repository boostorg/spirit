/*=============================================================================
    Copyright (c) 2001-2009 Hartmut Kaiser

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(SPIRIT_QI_BOOL_SEP_29_2009_0709AM)
#define SPIRIT_QI_BOOL_SEP_29_2009_0709AM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/qi/skip_over.hpp>
#include <boost/spirit/home/qi/meta_compiler.hpp>
#include <boost/spirit/home/qi/parser.hpp>
#include <boost/spirit/home/qi/numeric/bool_policies.hpp>
#include <boost/spirit/home/support/common_terminals.hpp>
#include <boost/spirit/home/support/info.hpp>
#include <boost/mpl/assert.hpp>

namespace boost { namespace spirit
{
    ///////////////////////////////////////////////////////////////////////////
    // Enablers
    ///////////////////////////////////////////////////////////////////////////
    template <>
    struct use_terminal<qi::domain, tag::bool_> // enables bool_
      : mpl::true_ {};
}}

namespace boost { namespace spirit { namespace qi
{
    using spirit::bool_;
    using spirit::bool__type;

    namespace detail
    {
        template <typename T, typename Policies>
        struct bool_impl
        {
            template <typename Iterator, typename Attribute>
            static bool parse(Iterator& first, Iterator const& last
              , Attribute& attr, Policies const& p) 
            {
                if (first == last)
                    return false;

                return p.parse_true(first, last, attr) ||
                       p.parse_false(first, last, attr);
            }
        };
    }

    ///////////////////////////////////////////////////////////////////////////
    // This actual boolean parser
    ///////////////////////////////////////////////////////////////////////////
    template <
        typename T = bool
      , typename Policies = bool_policies<T> >
    struct bool_parser_impl
      : primitive_parser<bool_parser_impl<T, Policies> >
    {
        template <typename Context, typename Iterator>
        struct attribute
        {
            typedef T type;
        };

        template <typename Iterator, typename Context
          , typename Skipper, typename Attribute>
        bool parse(Iterator& first, Iterator const& last
          , Context& /*context*/, Skipper const& skipper
          , Attribute& attr) const
        {
            qi::skip_over(first, last, skipper);
            return detail::bool_impl<T, Policies>::
                parse(first, last, attr, Policies());
        }

        template <typename Context>
        info what(Context& /*context*/) const
        {
            return info("boolean");
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    // uint_parser is the class that the user can instantiate directly
    ///////////////////////////////////////////////////////////////////////////
    template <
        typename T
      , typename Policies = bool_policies<T> >
    struct bool_parser
      : proto::terminal<bool_parser_impl<T, Policies> >::type
    {
    };

    ///////////////////////////////////////////////////////////////////////////
    // Parser generators: make_xxx function (objects)
    ///////////////////////////////////////////////////////////////////////////
    template <typename Modifiers>
    struct make_primitive<tag::bool_, Modifiers>
    {
        typedef bool_parser_impl<bool> result_type;
        result_type operator()(unused_type, unused_type) const
        {
            return result_type();
        }
    };

}}}

#endif
