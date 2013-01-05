/*=============================================================================
    Copyright (c) 2001-2012 Joel de Guzman
    Copyright (c)      2011 Bryce Lelbach

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_SPIRIT_INT_APR_17_2006_0830AM)
#define BOOST_SPIRIT_INT_APR_17_2006_0830AM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/x3/core/skip_over.hpp>
#include <boost/spirit/home/x3/numeric/numeric_utils.hpp>
#include <boost/spirit/home/x3/core/compiler.hpp>
#include <boost/spirit/home/x3/core/parser.hpp>
#include <boost/spirit/home/x3/core/lit.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/type_traits/is_same.hpp>

namespace boost { namespace spirit { namespace x3
{
    ///////////////////////////////////////////////////////////////////////
    // This one is the class that the user can instantiate directly in
    // order to create a customized int parser
    template <
        typename T = int, unsigned Radix = 10, unsigned MinDigits = 1
      , int MaxDigits = -1>
    struct int_parser
      : spirit::terminal<int_parser<T, Radix, MinDigits, MaxDigits>>
    {};

    struct short_type : terminal<short_type> {};
    struct int_type : terminal<int_type> {};
    struct long_type : terminal<long_type> {};
    struct long_long_type : terminal<long_long_type> {};

#ifndef BOOST_SPIRIT_NO_PREDEFINED_TERMINALS
    short_type const short_ = short_type();
    int_type const int_ = int_type();
    long_type const long_ = long_type();
    long_long_type const long_long = long_long_type();
#endif

    ///////////////////////////////////////////////////////////////////////////
    // This is the actual int parser
    ///////////////////////////////////////////////////////////////////////////
    //[primitive_parsers_int_parser
    template <
        typename T, unsigned Radix = 10, unsigned MinDigits = 1
      , int MaxDigits = -1>
    struct any_int_parser
      : parser<any_int_parser<T, Radix, MinDigits, MaxDigits>>
    {
        // check template parameter 'Radix' for validity
        static_assert(
            (Radix == 2 || Radix == 8 || Radix == 10 || Radix == 16),
            "Error Unsupported Radix");

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
            typedef extract_int<T, Radix, MinDigits, MaxDigits> extract;
            x3::skip_over(first, last, skipper);
            return extract::call(first, last, attr);
        }
    };
    //]

    template <typename T, unsigned Radix, unsigned MinDigits, int MaxDigits>
    struct get_info<any_int_parser<T, Radix, MinDigits, MaxDigits>>
    {
        typedef std::string result_type;

        template <typename P>
        std::string operator()(P const& p) const
        {
            switch (Radix)
            {
                case 2: return "binary integer";
                case 8: return "octal integer";
                case 16: return "hexadecimal integer";
            }
            return "integer";
        }
    };

    template <
        typename T, unsigned Radix = 10, unsigned MinDigits = 1
      , int MaxDigits = -1, bool no_attribute = true>
    struct literal_int_parser
      : parser<literal_int_parser<T, Radix, MinDigits, MaxDigits
      , no_attribute>>
    {
        // check template parameter 'Radix' for validity
        static_assert(
            (Radix == 2 || Radix == 8 || Radix == 10 || Radix == 16),
            "Error Unsupported Radix");

        template <typename Value>
        literal_int_parser(Value const& n) : n_(n) {}

        template <typename Context, typename Iterator>
        struct attribute
          : mpl::if_c<no_attribute, unused_type, T>
        {};

        template <typename Iterator, typename Context
          , typename Skipper, typename Attribute>
        bool parse(Iterator& first, Iterator const& last
          , Context& /*context*/, Skipper const& skipper
          , Attribute& attr) const
        {
            typedef extract_int<T, Radix, MinDigits, MaxDigits> extract;
            x3::skip_over(first, last, skipper);

            Iterator save = first;
            T attr_;

            if (extract::call(first, last, attr_) && (attr_ == n_))
            {
                traits::assign_to(attr_, attr);
                return true;
            }

            first = save;
            return false;
        }

        T n_;
    };

    template <
        typename T, unsigned Radix, unsigned MinDigits
      , int MaxDigits, bool no_attribute>
    struct get_info<literal_int_parser<T, Radix, MinDigits, MaxDigits, no_attribute>>
    {
        typedef std::string result_type;

        template <typename P>
        std::string operator()(P const& p) const
        {
            switch (Radix)
            {
                case 2: return "binary integer";
                case 8: return "octal integer";
                case 16: return "hexadecimal integer";
            }
            return "integer";
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    // Parser compilers
    ///////////////////////////////////////////////////////////////////////////
    //[primitive_parsers_make_int
    template <
        typename T, unsigned Radix = 10, unsigned MinDigits = 1
      , int MaxDigits = -1>
    struct make_int
    {
        typedef any_int_parser<T, Radix, MinDigits, MaxDigits> result_type;
        result_type operator()(unused_type) const
        {
            return result_type();
        }
    };
    //]

    template <
        typename T, unsigned Radix = 10, unsigned MinDigits = 1
      , int MaxDigits = -1>
    struct make_direct_int
    {
        typedef
            literal_int_parser<T, Radix, MinDigits, MaxDigits, false>
        result_type;

        template <typename Terminal>
        result_type operator()(Terminal const& term) const
        {
            return result_type(fusion::at_c<0>(term.args));
        }
    };

    template <
        typename T, unsigned Radix = 10, unsigned MinDigits = 1
      , int MaxDigits = -1>
    struct make_literal_int
    {
        typedef literal_int_parser<T, Radix, MinDigits, MaxDigits> result_type;
        template <typename Terminal>
        result_type operator()(Terminal const& term) const
        {
            return result_type(fusion::at_c<0>(term.args));
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    template <>
    struct compiler<
          terminal_ex<lit_type, fusion::vector1<signed short>>>
      : make_literal_int<signed short> {};

    template <>
    struct compiler<
          terminal_ex<lit_type, fusion::vector1<signed int>>>
      : make_literal_int<signed int> {};

    template <>
    struct compiler<
          terminal_ex<lit_type, fusion::vector1<signed long>>>
      : make_literal_int<signed long> {};

    template <>
    struct compiler<
          terminal_ex<lit_type, fusion::vector1<boost::long_long_type>>>
      : make_literal_int<boost::long_long_type> {};

    ///////////////////////////////////////////////////////////////////////////
    template <typename T, unsigned Radix, unsigned MinDigits, int MaxDigits>
    struct compiler<int_parser<T, Radix, MinDigits, MaxDigits>>
      : make_int<T, Radix, MinDigits, MaxDigits> {};

    template <
        typename T, unsigned Radix, unsigned MinDigits, int MaxDigits
      , typename A0>
    struct compiler<
        terminal_ex<int_parser<T, Radix, MinDigits, MaxDigits>, fusion::vector1<A0>>>
      : make_direct_int<T, Radix, MinDigits, MaxDigits> {};

    ///////////////////////////////////////////////////////////////////////////
    //[primitive_parsers_short_primitive
    template <>
    struct compiler<short_type>
      : make_int<short> {};
    //]

    template <typename A0>
    struct compiler<terminal_ex<short_type, fusion::vector1<A0>>>
      : make_direct_int<short> {};

    ///////////////////////////////////////////////////////////////////////////
    //[primitive_parsers_int_primitive
    template <>
    struct compiler<int_type>
      : make_int<int> {};
    //]

    template <typename A0>
    struct compiler<terminal_ex<int_type, fusion::vector1<A0>>>
      : make_direct_int<int> {};

    ///////////////////////////////////////////////////////////////////////////
    //[primitive_parsers_long_primitive
    template <>
    struct compiler<long_type>
      : make_int<long> {};
    //]

    template <typename A0>
    struct compiler<terminal_ex<long_type, fusion::vector1<A0>>>
      : make_direct_int<long> {};

    ///////////////////////////////////////////////////////////////////////////
    //[primitive_parsers_long_long_primitive
    template <>
    struct compiler<long_long_type>
      : make_int<boost::long_long_type> {};
    //]

    template <typename A0>
    struct compiler<terminal_ex<long_long_type, fusion::vector1<A0>>>
      : make_direct_int<boost::long_long_type> {};
}}}

#endif
