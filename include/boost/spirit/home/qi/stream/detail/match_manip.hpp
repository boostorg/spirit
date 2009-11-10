/*=============================================================================
    Copyright (c) 2001-2009 Hartmut Kaiser

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_SPIRIT_MATCH_MANIP_MAY_05_2007_1203PM)
#define BOOST_SPIRIT_MATCH_MANIP_MAY_05_2007_1203PM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/qi/parse.hpp>
#include <boost/spirit/home/support/unused.hpp>
#include <boost/mpl/bool.hpp>

#include <iterator>
#include <string>

///////////////////////////////////////////////////////////////////////////////
namespace boost { namespace spirit { namespace qi { namespace detail
{
    ///////////////////////////////////////////////////////////////////////////
    template <typename Expr, typename Copy = mpl::false_
      , typename Skipper = unused_type, typename Attribute = unused_type const>
    struct match_manip
    {
        match_manip(Expr const& xpr, Skipper const& s, Attribute& a)
          : expr(xpr), skipper(s), attr(a), post_skip(skip_flag::postskip) {}

        match_manip(Expr const& xpr, Skipper const& s
            , BOOST_SCOPED_ENUM(skip_flag) ps, Attribute& a)
          : expr(xpr), skipper(s), attr(a), post_skip(ps) {}

        Expr const& expr;
        Skipper const& skipper;
        Attribute& attr;
        BOOST_SCOPED_ENUM(skip_flag) const post_skip;

    private:
        // silence MSVC warning C4512: assignment operator could not be generated
        match_manip& operator= (match_manip const&);
    };

    template <typename Expr, typename Skipper, typename Attribute>
    struct match_manip<Expr, mpl::true_, Skipper, Attribute>
    {
        match_manip(Expr const& xpr, Skipper const& s, Attribute& a)
          : expr(xpr), skipper(s), attr(a), post_skip(skip_flag::postskip) {}

        match_manip(Expr const& xpr, Skipper const& s
            , BOOST_SCOPED_ENUM(skip_flag) ps, Attribute& a)
          : expr(xpr), skipper(s), attr(a), post_skip(ps) {}

        Expr const& expr;
        Skipper const& skipper;
        Attribute attr;
        BOOST_SCOPED_ENUM(skip_flag) const post_skip;

    private:
        // silence MSVC warning C4512: assignment operator could not be generated
        match_manip& operator= (match_manip const&);
    };

    ///////////////////////////////////////////////////////////////////////////
    template<typename Char, typename Traits, typename Expr, typename Copy>
    inline std::basic_istream<Char, Traits> &
    operator>>(std::basic_istream<Char, Traits> &is,
        match_manip<Expr, Copy> const& fm)
    {
        typedef std::istream_iterator<Char, Char, Traits> input_iterator;
        input_iterator f(is);
        input_iterator l;
        if (!qi::parse(f, l, fm.expr))
        {
            is.setstate(std::ios_base::failbit);
        }
        return is;
    }

    ///////////////////////////////////////////////////////////////////////////
    template<typename Char, typename Traits, typename Expr, typename Copy
      , typename Attribute>
    inline std::basic_istream<Char, Traits> &
    operator>>(std::basic_istream<Char, Traits> &is,
        match_manip<Expr, Copy, unused_type, Attribute> const& fm)
    {
        typedef std::istream_iterator<Char, Char, Traits> input_iterator;
        input_iterator f(is);
        input_iterator l;
        if (!qi::parse(f, l, fm.expr, fm.attr))
        {
            is.setstate(std::ios_base::failbit);
        }
        return is;
    }

    ///////////////////////////////////////////////////////////////////////////
    template<typename Char, typename Traits, typename Expr, typename Copy
      , typename Skipper>
    inline std::basic_istream<Char, Traits> &
    operator>>(std::basic_istream<Char, Traits> &is,
        match_manip<Expr, Copy, Skipper> const& fm)
    {
        typedef std::istream_iterator<Char, Char, Traits> input_iterator;
        input_iterator f(is);
        input_iterator l;
        if (!qi::phrase_parse(
                f, l, fm.expr, fm.skipper, fm.post_skip))
        {
            is.setstate(std::ios_base::failbit);
        }
        return is;
    }

    ///////////////////////////////////////////////////////////////////////////
    template<typename Char, typename Traits, typename Expr, typename Copy
      , typename Attribute, typename Skipper
    >
    inline std::basic_istream<Char, Traits> &
    operator>>(
        std::basic_istream<Char, Traits> &is,
        match_manip<Expr, Copy, Attribute, Skipper> const& fm)
    {
        typedef std::istream_iterator<Char, Char, Traits> input_iterator;
        input_iterator f(is);
        input_iterator l;
        if (!qi::phrase_parse(
                f, l, fm.expr, fm.skipper, fm.post_skip, fm.attr))
        {
            is.setstate(std::ios_base::failbit);
        }
        return is;
    }

}}}}

#endif
