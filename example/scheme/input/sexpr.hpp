/*=============================================================================
    Copyright (c) 2001-2010 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_SEXPR)
#define BOOST_SPIRIT_SEXPR

#include <string>

#include <boost/cstdint.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_container.hpp>
#include <boost/spirit/include/phoenix_statement.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/regex/pending/unicode_iterator.hpp>

#include "../utree.hpp"
#include "string.hpp"

namespace scheme { namespace input
{
    using boost::spirit::ascii::char_;
    using boost::spirit::ascii::space;
    using boost::spirit::qi::grammar;
    using boost::spirit::qi::rule;
    using boost::spirit::qi::eol;
    using boost::spirit::qi::uint_parser;
    using boost::spirit::qi::real_parser;
    using boost::spirit::qi::strict_real_policies;
    using boost::spirit::qi::int_;
    using boost::spirit::qi::hex;
    using boost::spirit::qi::oct;
    using boost::spirit::qi::bool_;
    using boost::spirit::qi::no_case;
    using boost::spirit::qi::lexeme;
    using boost::phoenix::function;

    typedef boost::uint32_t uchar; // a unicode code point

    template <typename Iterator>
    struct sexpr_white_space : grammar<Iterator>
    {
        sexpr_white_space() : sexpr_white_space::base_type(start)
        {
            start =
                    space                           // tab/space/cr/lf
                |   ';' >> *(char_ - eol) >> eol    // comments
                ;
        }

        rule<Iterator> start;
    };

    template <typename Iterator>
    struct sexpr : grammar<Iterator, sexpr_white_space<Iterator>, utree()>
    {
        sexpr() : sexpr::base_type(start)
        {
            real_parser<double, strict_real_policies<double> > strict_double;
            uint_parser<unsigned char, 16, 2, 2> hex2;

            start   = atom | list;

            list    = '(' >> *start >> ')';

            atom    = strict_double
                    | integer
                    | bool_
                    | string
                    | byte_str
                    | symbol
                    ;

            std::string exclude = std::string(" ();\"\x01-\x1f\x7f") + '\0';
            symbol  = lexeme[+(~char_(exclude))];

            integer = lexeme[no_case["0x"] >> hex]
                    | lexeme['0' >> oct]
                    | int_
                    ;

            byte_str = lexeme[no_case['b'] >> +hex2];
        }

        rule<Iterator, sexpr_white_space<Iterator>, utree()> start, list;
        rule<Iterator, int()> integer;
        rule<Iterator, utree()> atom;
        rule<Iterator, utf8_symbol()> symbol;
        rule<Iterator, binary_string()> byte_str;
        scheme::input::string<Iterator> string;
    };
}}

#endif
