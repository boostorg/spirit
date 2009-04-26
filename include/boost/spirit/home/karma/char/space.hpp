//  Copyright (c) 2001-2009 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(BOOST_SPIRIT_KARMA_SPACE_MAR_06_2007_0934PM)
#define BOOST_SPIRIT_KARMA_SPACE_MAR_06_2007_0934PM

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once      // MS compatible compilers support #pragma once
#endif

#include <boost/spirit/home/support/common_terminals.hpp>
#include <boost/spirit/home/support/string_traits.hpp>
#include <boost/spirit/home/support/info.hpp>
#include <boost/spirit/home/support/char_class.hpp>
#include <boost/spirit/home/support/detail/get_encoding.hpp>
#include <boost/spirit/home/karma/domain.hpp>
#include <boost/spirit/home/karma/meta_compiler.hpp>
#include <boost/spirit/home/karma/delimit_out.hpp>
#include <boost/spirit/home/karma/auxiliary/lazy.hpp>
#include <boost/spirit/home/karma/detail/get_casetag.hpp>
#include <boost/spirit/home/karma/detail/generate_to.hpp>

///////////////////////////////////////////////////////////////////////////////
namespace boost { namespace spirit 
{ 
    ///////////////////////////////////////////////////////////////////////////
    // Enablers
    ///////////////////////////////////////////////////////////////////////////
    template <typename CharEncoding>
    struct use_terminal<karma::domain
      , tag::char_code<tag::space, CharEncoding>        // enables space
    > : mpl::true_ {};

}}

///////////////////////////////////////////////////////////////////////////////
namespace boost { namespace spirit { namespace karma
{
    ///////////////////////////////////////////////////////////////////////////
    //
    //  space
    //      generates a single character from the associated parameter
    //
    ///////////////////////////////////////////////////////////////////////////
    template <typename CharEncoding>
    struct any_space
      : primitive_generator<any_space<CharEncoding> >
    {
        typedef typename CharEncoding::char_type char_type;
        typedef CharEncoding char_encoding;

        template <typename Context, typename Unused>
        struct attribute
        {
            typedef char_type type;
        };

        // any_space has an attached parameter
        template <
            typename OutputIterator, typename Context, typename Delimiter
          , typename Attribute>
        static bool generate(OutputIterator& sink, Context&, Delimiter const& d
          , Attribute const& attr)
        {
            if (!spirit::char_class::classify<char_encoding>::is(tag::space(), attr))
                return false;     // allow whitespace only

            return 
                karma::detail::generate_to(sink, attr) &&
                karma::delimit_out(sink, d);      // always do post-delimiting
        }

        // any_space has no attribute attached, use single space character
        template <typename OutputIterator, typename Context, typename Delimiter>
        static bool generate(OutputIterator& sink, Context&, Delimiter const& d, 
            unused_type const&)
        {
            return 
                karma::detail::generate_to(sink, ' ') &&
                karma::delimit_out(sink, d);      // always do post-delimiting
        }

        template <typename Context>
        static info what(Context const& ctx)
        {
            return info("space");
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    // Generator generators: make_xxx function (objects)
    ///////////////////////////////////////////////////////////////////////////
    // space
    template <typename CharEncoding, typename Modifiers>
    struct make_primitive<tag::char_code<tag::space, CharEncoding>, Modifiers>
    {
        typedef any_space<CharEncoding> result_type;

        result_type operator()(unused_type, unused_type) const
        {
            return result_type();
        }
    };

}}}  // namespace boost::spirit::karma

#endif // !defined(BOOST_SPIRIT_KARMA_CHAR_FEB_21_2007_0543PM)
