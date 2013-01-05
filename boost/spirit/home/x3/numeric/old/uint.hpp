/*=============================================================================
    Copyright (c) 2001-2012 Joel de Guzman
    Copyright (c) 2011 Bryce Lelbach
    Copyright (c) 2011 Jan Frederick Eick

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(SPIRIT_UINT_APR_17_2006_0901AM)
#define SPIRIT_UINT_APR_17_2006_0901AM

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
    // order to create a customized uint parser
    template <
        typename T = int, unsigned Radix = 10, unsigned MinDigits = 1
      , int MaxDigits = -1>
    struct uint_parser
      : terminal<uint_parser<T, Radix, MinDigits, MaxDigits>>
    {};

    struct bin_type: terminal<bin_type> {};
    struct oct_type: terminal<oct_type> {};
    struct hex_type : terminal<hex_type> {};

    struct ushort_type : terminal<ushort_type> {};
    struct uint_type : terminal<uint_type> {};
    struct ulong_type : terminal<ulong_type> {};
    struct ulong_long_type : terminal<ulong_long_type> {};

#ifndef BOOST_SPIRIT_NO_PREDEFINED_TERMINALS
    bin_type const bin = bin_type();
    oct_type const oct = oct_type();
    hex_type const hex = hex_type();

    ushort_type const ushort_ = ushort_type();
    uint_type const uint_ = uint_type();
    ulong_type const ulong_ = ulong_type();
    ulong_long_type const ulong_long = ulong_long_type();
#endif

    ///////////////////////////////////////////////////////////////////////////
    // This is the actual uint parser
    ///////////////////////////////////////////////////////////////////////////
    template <
        typename T, unsigned Radix = 10, unsigned MinDigits = 1
      , int MaxDigits = -1>
    struct any_uint_parser
      : parser<any_uint_parser<T, Radix, MinDigits, MaxDigits>>
    {
        // check template parameter 'Radix' for validity
        static_assert(
            (Radix >= 2 && Radix <= 36),
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
            typedef extract_uint<T, Radix, MinDigits, MaxDigits> extract;
            x3::skip_over(first, last, skipper);
            return extract::call(first, last, attr);
        }
    };
    //]

    template <typename T, unsigned Radix, unsigned MinDigits, int MaxDigits>
    struct get_info<any_uint_parser<T, Radix, MinDigits, MaxDigits>>
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
            return "unsigned integer";
        }
    };

    template <
        typename T, unsigned Radix = 10, unsigned MinDigits = 1
      , int MaxDigits = -1, bool no_attribute = true>
    struct literal_uint_parser
      : parser<literal_uint_parser<T, Radix, MinDigits, MaxDigits
        , no_attribute>>
    {
        // check template parameter 'Radix' for validity
        static_assert(
            (Radix == 2 || Radix == 8 || Radix == 10 || Radix == 16),
            "Error Unsupported Radix");

        template <typename Value>
        literal_uint_parser(Value const& n) : n_(n) {}

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
            typedef extract_uint<T, Radix, MinDigits, MaxDigits> extract;
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
    struct get_info<literal_uint_parser<T, Radix, MinDigits, MaxDigits, no_attribute>>
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
            return "unsigned integer";
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    // Parser compilers
    ///////////////////////////////////////////////////////////////////////////
    template <
        typename T, unsigned Radix = 10, unsigned MinDigits = 1
      , int MaxDigits = -1>
    struct make_uint
    {
        typedef any_uint_parser<T, Radix, MinDigits, MaxDigits> result_type;
        result_type operator()(unused_type) const
        {
            return result_type();
        }
    };

    template <
        typename T, unsigned Radix = 10, unsigned MinDigits = 1
      , int MaxDigits = -1>
    struct make_direct_uint
    {
        typedef literal_uint_parser<T, Radix, MinDigits, MaxDigits, false>
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
    struct make_literal_uint
    {
        typedef literal_uint_parser<T, Radix, MinDigits, MaxDigits> result_type;
        template <typename Terminal>
        result_type operator()(Terminal const& term) const
        {
            return result_type(fusion::at_c<0>(term.args));
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    template <>
    struct compiler<
          terminal_ex<lit_type, fusion::vector1<unsigned short>>>
      : make_literal_uint<unsigned short> {};

    template <>
    struct compiler<
          terminal_ex<lit_type, fusion::vector1<unsigned int>>>
      : make_literal_uint<unsigned int> {};

    template <>
    struct compiler<
          terminal_ex<lit_type, fusion::vector1<unsigned long>>>
      : make_literal_uint<unsigned long> {};

    template <>
    struct compiler<
          terminal_ex<lit_type, fusion::vector1<boost::ulong_long_type>>>
      : make_literal_uint<boost::ulong_long_type> {};

    ///////////////////////////////////////////////////////////////////////////
    template <typename T, unsigned Radix, unsigned MinDigits, int MaxDigits>
    struct compiler<x3::uint_parser<T, Radix, MinDigits, MaxDigits>>
      : make_uint<T, Radix, MinDigits, MaxDigits> {};

    template <
        typename T, unsigned Radix, unsigned MinDigits, int MaxDigits
      , typename A0>
    struct compiler<
        terminal_ex<
            uint_parser<T, Radix, MinDigits, MaxDigits>, fusion::vector1<A0>>>
      : make_direct_uint<T, Radix, MinDigits, MaxDigits> {};

    ///////////////////////////////////////////////////////////////////////////
    template <>
    struct compiler<bin_type>
      : make_uint<unsigned, 2> {};

    template <typename A0>
    struct compiler<terminal_ex<bin_type, fusion::vector1<A0>>>
      : make_direct_uint<unsigned, 2> {};

    ///////////////////////////////////////////////////////////////////////////
    template <>
    struct compiler<oct_type>
      : make_uint<unsigned, 8> {};

    template <typename A0>
    struct compiler<terminal_ex<oct_type, fusion::vector1<A0>>>
      : make_direct_uint<unsigned, 8> {};

    ///////////////////////////////////////////////////////////////////////////
    template <>
    struct compiler<hex_type>
      : make_uint<unsigned, 16> {};

    template <typename A0>
    struct compiler<terminal_ex<hex_type, fusion::vector1<A0>>>
      : make_direct_uint<unsigned, 16> {};

    ///////////////////////////////////////////////////////////////////////////
    template <>
    struct compiler<ushort_type>
      : make_uint<unsigned short> {};

    template <typename A0>
    struct compiler<terminal_ex<ushort_type, fusion::vector1<A0>>>
      : make_direct_uint<unsigned short> {};

    ///////////////////////////////////////////////////////////////////////////
    template <>
    struct compiler<uint_type>
      : make_uint<unsigned> {};

    template <typename A0>
    struct compiler<terminal_ex<uint_type, fusion::vector1<A0>>>
      : make_direct_uint<unsigned> {};

    ///////////////////////////////////////////////////////////////////////////
    template <>
    struct compiler<ulong_type>
      : make_uint<unsigned long> {};

    template <typename A0>
    struct compiler<terminal_ex<ulong_type, fusion::vector1<A0>>>
      : make_direct_uint<unsigned long> {};

    ///////////////////////////////////////////////////////////////////////////
    template <>
    struct compiler<ulong_long_type>
      : make_uint<boost::ulong_long_type> {};

    template <typename A0>
    struct compiler<terminal_ex<ulong_long_type, fusion::vector1<A0>>>
      : make_direct_uint<boost::ulong_long_type> {};
}}}

#endif
