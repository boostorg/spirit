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

namespace scheme { namespace input
{
    using boost::spirit::ascii::char_;
    using boost::spirit::ascii::space;
    using boost::spirit::qi::grammar;
    using boost::spirit::qi::rule;
    using boost::spirit::qi::eol;
    using boost::spirit::qi::_val;
    using boost::spirit::qi::_r1;
    using boost::spirit::qi::_1;
    using boost::spirit::qi::uint_parser;
    using boost::spirit::qi::real_parser;
    using boost::spirit::qi::strict_real_policies;
    using boost::spirit::qi::char_set;
    using boost::spirit::qi::int_;
    using boost::spirit::qi::hex;
    using boost::spirit::qi::oct;
    using boost::spirit::qi::bool_;
    using boost::spirit::qi::no_case;
    using boost::spirit::qi::lexeme;
    using boost::spirit::qi::lit;
    using boost::phoenix::function;

    typedef boost::uint32_t uchar; // a unicode code point

    template <typename Iterator>
    struct white_space : grammar<Iterator>
    {
        white_space() : white_space::base_type(start)
        {
            start =
                    space                           // tab/space/cr/lf
                |   ';' >> *(char_ - eol) >> eol    // comments
                ;
        }

        rule<Iterator> start;
    };

    namespace detail
    {
        struct push_utf8
        {
            template <typename S, typename C>
            struct result { typedef void type; };

            void operator()(std::string& utf8, uchar code_point) const
            {
                typedef std::back_insert_iterator<std::string> insert_iter;
                insert_iter out_iter(utf8);
                boost::utf8_output_iterator<insert_iter> utf8_iter(out_iter);
                *utf8_iter++ = code_point;
            }
        };

        struct push_esc
        {
            template <typename S, typename C>
            struct result { typedef void type; };

            void operator()(std::string& utf8, uchar c) const
            {
                switch (c)
                {
                    case 'b': utf8 += '\b';     break;
                    case 't': utf8 += '\t';     break;
                    case 'n': utf8 += '\n';     break;
                    case 'f': utf8 += '\f';     break;
                    case 'r': utf8 += '\r';     break;
                    case '"': utf8 += '"';      break;
                    case '\\': utf8 += '\\';    break;
                }
            }
        };
    }

    template <typename Iterator>
    struct string : grammar<Iterator, std::string()>
    {
        string() : string::base_type(start)
        {
            uint_parser<uchar, 16, 4, 4> hex4;
            uint_parser<uchar, 16, 8, 8> hex8;
            function<detail::push_utf8> push_utf8;
            function<detail::push_esc> push_esc;

            str_esc
                =  '\\'
                >>  (   ('u' >> hex4)               [push_utf8(_r1, _1)]
                    |   ('U' >> hex8)               [push_utf8(_r1, _1)]
                    |   char_("btnfr\\\"'")         [push_esc(_r1, _1)]
                    )
                ;

            start
                = '"'
                >> *(str_esc(_val) | (~char_('"'))  [_val += _1])
                >> '"'
                ;
        }

        rule<Iterator, void(std::string&)> str_esc;
        rule<Iterator, std::string()> start;
    };

    template <typename Iterator>
    struct sexpr : grammar<Iterator, white_space<Iterator>, utree()>
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

            char const* exclude = " ();\"\0-\31\127";
            symbol  = lexeme[+(~char_(exclude))];

            integer = lexeme[no_case["0x"] >> hex]
                    | lexeme['0' >> oct]
                    | int_
                    ;

            byte_str = lexeme[no_case['b'] >> +hex2];
        }

        rule<Iterator, white_space<Iterator>, utree()> start, list;
        rule<Iterator, int()> integer;
        rule<Iterator, utree()> atom;
        rule<Iterator, utf8_symbol()> symbol;
        rule<Iterator, binary_string()> byte_str;
        scheme::input::string<Iterator> string;
    };
}}

#endif
