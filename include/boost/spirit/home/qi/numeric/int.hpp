/*=============================================================================
    Copyright (c) 2001-2009 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_SPIRIT_INT_APR_17_2006_0830AM)
#define BOOST_SPIRIT_INT_APR_17_2006_0830AM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/qi/skip_over.hpp>
#include <boost/spirit/home/qi/numeric/numeric_utils.hpp>
#include <boost/spirit/home/qi/meta_compiler.hpp>
#include <boost/spirit/home/qi/parser.hpp>
#include <boost/spirit/home/support/common_terminals.hpp>
#include <boost/spirit/home/support/info.hpp>
#include <boost/mpl/assert.hpp>

namespace boost { namespace spirit
{
    ///////////////////////////////////////////////////////////////////////////
    // Enablers
    ///////////////////////////////////////////////////////////////////////////
    template <>
    struct use_terminal<qi::domain, tag::short_> // enables short_
      : mpl::true_ {};

    template <>
    struct use_terminal<qi::domain, tag::int_> // enables int_
      : mpl::true_ {};

    template <>
    struct use_terminal<qi::domain, tag::long_> // enables long_
      : mpl::true_ {};

#ifdef BOOST_HAS_LONG_LONG
    template <>
    struct use_terminal<qi::domain, tag::long_long> // enables long_long
      : mpl::true_ {};
#endif
}}

namespace boost { namespace spirit { namespace qi
{
    using spirit::short_;
    using spirit::short__type;
    using spirit::int_;
    using spirit::int__type;
    using spirit::long_;
    using spirit::long__type;
#ifdef BOOST_HAS_LONG_LONG
    using spirit::long_long;
    using spirit::long_long_type;
#endif

    ///////////////////////////////////////////////////////////////////////////
    // This is the actual int parser
    ///////////////////////////////////////////////////////////////////////////
    template <
        typename T
      , unsigned Radix = 10
      , unsigned MinDigits = 1
      , int MaxDigits = -1>
    struct int_parser_impl
      : primitive_parser<int_parser_impl<T, Radix, MinDigits, MaxDigits> >
    {
        // check template parameter 'Radix' for validity
        BOOST_SPIRIT_ASSERT_MSG(
            Radix == 2 || Radix == 8 || Radix == 10 || Radix == 16,
            not_supported_radix, ());

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
            return extract_int<T, Radix, MinDigits, MaxDigits>
                ::call(first, last, attr);
        }

        template <typename Context>
        info what(Context& /*context*/) const
        {
            return info("integer");
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    // This one is the class that the user can instantiate directly
    ///////////////////////////////////////////////////////////////////////////
    template <
        typename T
      , unsigned Radix = 10
      , unsigned MinDigits = 1
      , int MaxDigits = -1>
    struct int_parser
      : proto::terminal<int_parser_impl<T, Radix, MinDigits, MaxDigits> >::type
    {
    };

    ///////////////////////////////////////////////////////////////////////////
    // Parser generators: make_xxx function (objects)
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    struct make_int
    {
        typedef int_parser_impl<T> result_type;
        result_type operator()(unused_type, unused_type) const
        {
            return result_type();
        }
    };

    template <typename Modifiers>
    struct make_primitive<tag::short_, Modifiers> : make_int<short> {};

    template <typename Modifiers>
    struct make_primitive<tag::int_, Modifiers> : make_int<int> {};

    template <typename Modifiers>
    struct make_primitive<tag::long_, Modifiers> : make_int<long> {};

#ifdef BOOST_HAS_LONG_LONG
    template <typename Modifiers>
    struct make_primitive<tag::long_long, Modifiers> : make_int<long long> {};
#endif

}}}

#endif
