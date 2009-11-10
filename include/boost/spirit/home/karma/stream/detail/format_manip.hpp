//  Copyright (c) 2001-2009 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(BOOST_SPIRIT_KARMA_FORMAT_MANIP_MAY_03_2007_1424PM)
#define BOOST_SPIRIT_KARMA_FORMAT_MANIP_MAY_03_2007_1424PM

#if defined(_MSC_VER)
#pragma once
#endif

#include <iterator>
#include <string>
#include <boost/spirit/home/karma/generate.hpp>
#include <boost/spirit/home/karma/stream/ostream_iterator.hpp>
#include <boost/mpl/bool.hpp>

///////////////////////////////////////////////////////////////////////////////
namespace boost { namespace spirit { namespace karma { namespace detail
{
    ///////////////////////////////////////////////////////////////////////////
    template <typename Expr, typename CopyAttribute = mpl::false_
      , typename Delimiter = unused_type, typename Attribute = unused_type>
    struct format_manip 
    {
        format_manip(Expr const& xpr, Delimiter const& d, Attribute const& a) 
          : expr(xpr), delim(d), pre(delimit_flag::dont_predelimit), attr(a) {}

        format_manip(Expr const& xpr, Delimiter const& d
            , BOOST_SCOPED_ENUM(delimit_flag) pre_delimit, Attribute const& a) 
          : expr(xpr), delim(d), pre(pre_delimit), attr(a) {}

        Expr const& expr;
        Delimiter const& delim;
        BOOST_SCOPED_ENUM(delimit_flag) const pre;
        Attribute const& attr;

    private:
        // silence MSVC warning C4512: assignment operator could not be generated
        format_manip& operator= (format_manip const&);
    };

    template <typename Expr, typename Delimiter, typename Attribute>
    struct format_manip<Expr, mpl::true_, Delimiter, Attribute>
    {
        format_manip(Expr const& xpr, Delimiter const& d, Attribute const& a) 
          : expr(xpr), delim(d), pre(delimit_flag::dont_predelimit), attr(a) {}

        format_manip(Expr const& xpr, Delimiter const& d
            , BOOST_SCOPED_ENUM(delimit_flag) pre_delimit, Attribute const& a) 
          : expr(xpr), delim(d), pre(pre_delimit), attr(a) {}

        Expr const& expr;
        Delimiter const& delim;
        BOOST_SCOPED_ENUM(delimit_flag) const pre;
        Attribute attr;

    private:
        // silence MSVC warning C4512: assignment operator could not be generated
        format_manip& operator= (format_manip const&);
    };

    ///////////////////////////////////////////////////////////////////////////
    template<typename Char, typename Traits, typename Expr, typename Copy> 
    inline std::basic_ostream<Char, Traits> & 
    operator<< (std::basic_ostream<Char, Traits> &os
      , format_manip<Expr, Copy> const& fm)
    {
        karma::ostream_iterator<Char, Char, Traits> sink(os);
        if (!karma::generate (sink, fm.expr))
        {
            os.setstate(std::ios_base::failbit);
        }
        return os;
    }

    ///////////////////////////////////////////////////////////////////////////
    template<typename Char, typename Traits, typename Expr, typename Copy
      , typename Attribute> 
    inline std::basic_ostream<Char, Traits> & 
    operator<< (std::basic_ostream<Char, Traits> &os
      , format_manip<Expr, Copy, unused_type, Attribute> const& fm)
    {
        karma::ostream_iterator<Char, Char, Traits> sink(os);
        if (!karma::generate(sink, fm.expr, fm.attr))
        {
            os.setstate(std::ios_base::failbit);
        }
        return os;
    }

    template<typename Char, typename Traits, typename Expr, typename Copy
      , typename Delimiter> 
    inline std::basic_ostream<Char, Traits> & 
    operator<< (std::basic_ostream<Char, Traits> &os
      , format_manip<Expr, Copy, Delimiter> const& fm)
    {
        karma::ostream_iterator<Char, Char, Traits> sink(os);
        if (!karma::generate_delimited(sink, fm.expr, fm.delim, fm.pre))
        {
            os.setstate(std::ios_base::failbit);
        }
        return os;
    }

    ///////////////////////////////////////////////////////////////////////////
    template<typename Char, typename Traits, typename Expr, typename Copy
      , typename Delimiter, typename Attribute> 
    inline std::basic_ostream<Char, Traits> & 
    operator<< (std::basic_ostream<Char, Traits> &os
      , format_manip<Expr, Copy, Delimiter, Attribute> const& fm)
    {
        karma::ostream_iterator<Char, Char, Traits> sink(os);
        if (!karma::generate_delimited(sink, fm.expr, fm.delim, fm.pre, fm.attr))
        {
            os.setstate(std::ios_base::failbit);
        }
        return os;
    }

}}}}

#endif
