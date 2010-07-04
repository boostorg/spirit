//  Copyright (c) 2001-2010 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(BOOST_SPIRIT_QIEXPR)
#define BOOST_SPIRIT_QIEXPR

#include <string>

#include <boost/cstdint.hpp>
#include <boost/detail/iterator.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>

#include "../utree.hpp"
#include "../utree_operators.hpp"
#include "string.hpp"

namespace scheme { namespace input
{
    using boost::spirit::ascii::space;
    using boost::spirit::qi::grammar;
    using boost::spirit::qi::rule;
    using boost::spirit::qi::symbols;
    using boost::spirit::qi::eol;
    using boost::spirit::qi::_val;
    using boost::spirit::qi::_1;
    using boost::phoenix::push_back;

    ///////////////////////////////////////////////////////////////////////////
    template <typename Iterator>
    struct qiexpr_white_space : grammar<Iterator>
    {
        qiexpr_white_space() : qiexpr_white_space::base_type(start)
        {
            start =
                    space                           // tab/space/cr/lf
                |   "//" >> *(char_ - eol) >> eol   // comments
                |   "/*" >> *(char_ - "*/") >> "*/"
                ;

//             start.name("ws"); debug(start);
        }

        rule<Iterator> start;
    };

    ///////////////////////////////////////////////////////////////////////////
    // a list of names for all supported parser primitives taking no parameters
    static char const* const primitives0[] = 
    {
        "char_"
      , "long_long", "long_", "int_", "short_"
      , "ulong_long", "ulong_", "uint_", "ushort_"
      , "bool_", "true_", "false_"
      , "long_double", "double_", "float_"
      , 0
    };

    // a list of names for all supported parser primitives taking 1 parameter
    static char const* const primitives1[] = 
    {
        "char_", "lit", "string"
      , 0
    };

    // a list of names for all supported parser primitives taking 2 parameter
    static char const* const primitives2[] = 
    {
        "char_"
      , 0
    };

    ///////////////////////////////////////////////////////////////////////////
    template <typename Iterator>
    struct qiexpr : grammar<Iterator, qiexpr_white_space<Iterator>, utree()>
    {
        typedef typename boost::detail::iterator_traits<Iterator>::value_type 
            char_type;

        qiexpr() : qiexpr::base_type(start)
        {
            start = *expr;

            expr =
                    primitive2 >> '(' >> literal >> ',' >> literal >> ')'
                |   primitive1 >> '(' >> literal >> ')'
                |   primitive0        // taking no parameter
                ;

            literal = 
                    string_lit            [ push_back(_val, _1) ]
                |   string_lit.char_lit   [ push_back(_val, _1) ]
                ;

            // fill the symbol tables with all known primitive parser names
            for (char const* const* p = primitives0; *p; ++p)
            {
                utree l;
                l.push_back(utf8_symbol(*p));
                primitive0.add(*p, l);
            }

            for (char const* const* p = primitives1; *p; ++p)
            {
                utree l;
                l.push_back(utf8_symbol(*p));
                primitive1.add(*p, l);
            }

            for (char const* const* p = primitives2; *p; ++p)
            {
                utree l;
                l.push_back(utf8_symbol(*p));
                primitive2.add(*p, l);
            }
        }

        rule<Iterator, qiexpr_white_space<Iterator>, utree()> start, expr, literal;
        symbols<char_type, utree> primitive0, primitive1, primitive2;
        scheme::input::string<Iterator> string_lit;
    };
}}

#endif
