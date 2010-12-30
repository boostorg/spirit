/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c)      2011 Bryce Lelbach

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(SPIRIT_UINT_APR_17_2006_0901AM)
#define SPIRIT_UINT_APR_17_2006_0901AM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/type_traits/is_arithmetic.hpp>
#include <boost/spirit/home/qi/skip_over.hpp>
#include <boost/spirit/home/qi/numeric/numeric_utils.hpp>
#include <boost/spirit/home/qi/meta_compiler.hpp>
#include <boost/spirit/home/qi/parser.hpp>
#include <boost/spirit/home/support/common_terminals.hpp>
#include <boost/spirit/home/support/info.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/preprocessor/seq.hpp>

namespace boost { namespace spirit
{
    ///////////////////////////////////////////////////////////////////////////
    // Enablers
    ///////////////////////////////////////////////////////////////////////////
    #define BOOST_SPIRIT_ENABLE_UINTEGER(r, data, name)                         \
        template <>                                                             \
        struct use_terminal<qi::domain, tag::name>                              \
          : mpl::true_ {};                                                      \
                                                                                \
        template <typename A0>                                                  \
        struct use_terminal<qi::domain                                          \
            , terminal_ex<tag::name, fusion::vector1<A0> > >                    \
          : is_arithmetic<A0> {};                                               \
                                                                                \
        template <>                                                             \
        struct use_lazy_terminal<qi::domain, tag::name, 1> : mpl::true_ {};     \
    /***/

    #define BOOST_SPIRIT_ENABLE_UINTEGERS(names)                                \
        BOOST_PP_SEQ_FOR_EACH(BOOST_SPIRIT_ENABLE_UINTEGER, _, names)           \
    /***/

    BOOST_SPIRIT_ENABLE_UINTEGERS(
        (bin)
        (oct)
        (hex)
        (ushort_)
        (uint_)
        (ulong_)
        #ifdef BOOST_HAS_LONG_LONG
        (ulong_long)
        #endif
      )
}}

namespace boost { namespace spirit { namespace qi
{
    using spirit::bin;
    using spirit::bin_type;
    using spirit::oct;
    using spirit::oct_type;
    using spirit::hex;
    using spirit::hex_type;
    using spirit::ushort_;
    using spirit::ushort__type;
    using spirit::ulong_;
    using spirit::ulong__type;
    using spirit::uint_;
    using spirit::uint__type;
#ifdef BOOST_HAS_LONG_LONG
    using spirit::ulong_long;
    using spirit::ulong_long_type;
#endif

    ///////////////////////////////////////////////////////////////////////////
    // This is actual unsigned int parser
    ///////////////////////////////////////////////////////////////////////////
    template <
        typename T
      , unsigned Radix = 10
      , unsigned MinDigits = 1
      , int MaxDigits = -1
      , bool no_attribute = true
      , typename Enable = void>
    struct uint_parser_impl;

    template <
        typename T
      , unsigned Radix
      , unsigned MinDigits
      , int MaxDigits
      , bool no_attribute
      , typename Enable>
    struct uint_parser_impl
      : primitive_parser<uint_parser_impl<T, Radix, MinDigits, MaxDigits
        , no_attribute, Enable> >
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
            typedef extract_uint<T, Radix, MinDigits, MaxDigits> extract;
            qi::skip_over(first, last, skipper);
            return extract::call(first, last, attr);
        }

        template <typename Context>
        info what(Context& /*context*/) const
        {
            return info("unsigned-integer");
        }
    };

    template <
        typename T
      , unsigned Radix
      , unsigned MinDigits
      , int MaxDigits>
    struct uint_parser_impl<T, Radix, MinDigits, MaxDigits, false, typename
        enable_if<is_arithmetic<T> >::type>
      : primitive_parser<uint_parser_impl<T, Radix, MinDigits, MaxDigits
        , false> >
    {
        // check template parameter 'Radix' for validity
        BOOST_SPIRIT_ASSERT_MSG(
            Radix == 2 || Radix == 8 || Radix == 10 || Radix == 16,
            not_supported_radix, ());

        T n;

        template <typename Value>
        uint_parser_impl(Value const& v) : n(v) { }

        template <typename Context, typename Iterator>
        struct attribute
        {
            typedef unused_type type;
        };

        template <typename Iterator, typename Context
          , typename Skipper, typename Attribute>
        bool parse(Iterator& first, Iterator const& last
          , Context& /*context*/, Skipper const& skipper
          , Attribute& attr) const
        {
            typedef extract_uint<T, Radix, MinDigits, MaxDigits> extract;
            qi::skip_over(first, last, skipper);
            
            T attr_;

            if (extract::call(first, last, attr_) && (attr_ == n))
            {
                traits::assign_to(attr_, attr);
                return true;
            }

            return false;
        }

        template <typename Context>
        info what(Context& /*context*/) const
        {
            return info("literal-unsigned-integer");
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    // This is the class that the user can instantiate directly
    ///////////////////////////////////////////////////////////////////////////

    // FIXME: how can we allow user instantiated numeric parsers to be used as
    // literal numeric parsers?
    template <
        typename T
      , unsigned Radix = 10
      , unsigned MinDigits = 1
      , int MaxDigits = -1>
    struct uint_parser
      : proto::terminal<uint_parser_impl<T, Radix, MinDigits, MaxDigits> >::type
    {
    };

    ///////////////////////////////////////////////////////////////////////////
    // Parser generators: make_xxx function (objects)
    ///////////////////////////////////////////////////////////////////////////
    template <
        typename T
      , unsigned Radix = 10
      , unsigned MinDigits = 1
      , int MaxDigits = -1>
    struct make_uint
    {
        typedef uint_parser_impl<T, Radix, MinDigits, MaxDigits, true>
            result_type;
        result_type operator()(unused_type, unused_type) const
        {
            return result_type();
        }
    };
    
    template <
        typename T
      , unsigned Radix = 10
      , unsigned MinDigits = 1
      , int MaxDigits = -1>
    struct make_literal_uint
    {
        typedef uint_parser_impl<T, Radix, MinDigits, MaxDigits, false>
            result_type;
        template <typename Terminal>
        result_type operator()(Terminal const& term, unused_type) const
        {
            return result_type(fusion::at_c<0>(term.args));
        }
    };
    
    #define BOOST_SPIRIT_MAKE_UINTEGER_PRIMITIVE(r, data, elem)                 \
        template <typename Modifiers>                                           \
        struct make_primitive<tag::BOOST_PP_SEQ_ELEM(0, elem), Modifiers>       \
          : make_uint<BOOST_PP_SEQ_ELEM(1, elem)                                \
                    , BOOST_PP_SEQ_ELEM(2, elem)> {};                           \
                                                                                \
        template <typename Modifiers, typename A0>                              \
        struct make_primitive<                                                  \
            terminal_ex<tag::BOOST_PP_SEQ_ELEM(0, elem)                         \
          , fusion::vector1<A0> > , Modifiers>                                  \
          : make_literal_uint<BOOST_PP_SEQ_ELEM(1, elem)                        \
                            , BOOST_PP_SEQ_ELEM(2, elem)> {};                   \
    /***/

    #define BOOST_SPIRIT_MAKE_UINTEGER_PRIMITIVES(names)                        \
        BOOST_PP_SEQ_FOR_EACH(BOOST_SPIRIT_MAKE_UINTEGER_PRIMITIVE, _, names)   \
    /***/

    BOOST_SPIRIT_MAKE_UINTEGER_PRIMITIVES(
        ((bin)         (unsigned)       (2))
        ((oct)         (unsigned)       (8))
        ((hex)         (unsigned)       (16))
        ((ushort_)     (unsigned short) (10))
        ((uint_)       (unsigned int)   (10))
        ((ulong_)      (unsigned long)  (10))
        #ifdef BOOST_HAS_LONG_LONG
        ((ulong_long)  (boost::ulong_long_type) (10))
        #endif
    )
}}}

#endif
