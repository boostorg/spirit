/*=============================================================================
    Copyright (c) 2001-2009 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_PARSE_APRIL_16_2006_0442PM)
#define BOOST_SPIRIT_PARSE_APRIL_16_2006_0442PM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/qi/meta_compiler.hpp>
#include <boost/spirit/home/qi/skip_over.hpp>
#include <boost/spirit/home/support/unused.hpp>
#include <boost/spirit/home/support/detail/scoped_enum_emulation.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/mpl/bool.hpp>

namespace boost { namespace spirit { namespace qi
{
    ///////////////////////////////////////////////////////////////////////////
    BOOST_SCOPED_ENUM_START(skip_flag) 
    { 
        postskip,           // force post-skipping in phrase_parse()
        dont_postskip       // inhibit post-skipping in phrase_parse()
    };
    BOOST_SCOPED_ENUM_END

    ///////////////////////////////////////////////////////////////////////////
    template <typename Iterator, typename Expr>
    inline bool
    parse(
        Iterator& first
      , Iterator last
      , Expr const& expr)
    {
        // Report invalid expression error as early as possible.
        // If you got an error_invalid_expression error message here,
        // then the expression (expr) is not a valid spirit qi expression.
        BOOST_SPIRIT_ASSERT_MATCH(qi::domain, Expr);
        return compile<qi::domain>(expr).parse(first, last, unused, unused, unused);
    }

    template <typename Iterator, typename Expr, typename Attr>
    inline bool
    parse(
        Iterator& first
      , Iterator last
      , Expr const& expr
      , Attr& attr)
    {
        // Report invalid expression error as early as possible.
        // If you got an error_invalid_expression error message here,
        // then the expression (expr) is not a valid spirit qi expression.
        BOOST_SPIRIT_ASSERT_MATCH(qi::domain, Expr);
        return compile<qi::domain>(expr).parse(first, last, unused, unused, attr);
    }

    ///////////////////////////////////////////////////////////////////////////
    template <typename Iterator, typename Expr, typename Skipper>
    inline bool
    phrase_parse(
        Iterator& first
      , Iterator last
      , Expr const& expr
      , Skipper const& skipper
      , BOOST_SCOPED_ENUM(skip_flag) post_skip = skip_flag::postskip)
    {
        // Report invalid expression error as early as possible.
        // If you got an error_invalid_expression error message here,
        // then either the expression (expr) or skipper is not a valid
        // spirit qi expression.
        BOOST_SPIRIT_ASSERT_MATCH(qi::domain, Expr);
        BOOST_SPIRIT_ASSERT_MATCH(qi::domain, Skipper);

        typedef
            typename result_of::compile<qi::domain, Skipper>::type
        skipper_type;
        skipper_type const skipper_ = compile<qi::domain>(skipper);

        if (!compile<qi::domain>(expr).parse(
                first, last, unused, skipper_, unused))
            return false;

        if (post_skip == skip_flag::postskip)
            qi::skip_over(first, last, skipper_);
        return true;
    }

    template <typename Iterator, typename Expr, typename Skipper, typename Attr>
    inline bool
    phrase_parse(
        Iterator& first
      , Iterator last
      , Expr const& expr
      , Skipper const& skipper
      , BOOST_SCOPED_ENUM(skip_flag) post_skip
      , Attr& attr)
    {
        // Report invalid expression error as early as possible.
        // If you got an error_invalid_expression error message here,
        // then either the expression (expr) or skipper is not a valid
        // spirit qi expression.
        BOOST_SPIRIT_ASSERT_MATCH(qi::domain, Expr);
        BOOST_SPIRIT_ASSERT_MATCH(qi::domain, Skipper);

        typedef
            typename result_of::compile<qi::domain, Skipper>::type
        skipper_type;
        skipper_type const skipper_ = compile<qi::domain>(skipper);

        if (!compile<qi::domain>(expr).parse(
                first, last, unused, skipper_, attr))
            return false;

        if (post_skip == skip_flag::postskip)
            qi::skip_over(first, last, skipper_);
        return true;
    }

    template <typename Iterator, typename Expr, typename Skipper, typename Attr>
    inline bool
    phrase_parse(
        Iterator& first
      , Iterator last
      , Expr const& expr
      , Skipper const& skipper
      , Attr& attr)
    {
        return phrase_parse(first, last, expr, skipper, skip_flag::postskip, attr);
    }

}}}

#endif

