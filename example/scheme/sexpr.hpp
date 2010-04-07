/*=============================================================================
    Copyright (c) 2001-2010 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_SEXPR)
#define BOOST_SPIRIT_SEXPR

#include <string>

#define BOOST_SPIRIT_UNICODE // We'll use unicode (UTF8) all throughout

#include <boost/config/warning_disable.hpp>
#include <boost/cstdint.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_container.hpp>
#include <boost/spirit/include/phoenix_statement.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/regex/pending/unicode_iterator.hpp>

#include "utree.hpp"

namespace scheme
{
    using boost::spirit::unicode::char_;
    using boost::spirit::unicode::space;
    using boost::spirit::unicode::print;
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

    typedef boost::spirit::char_encoding::unicode unicode;
    typedef boost::uint32_t uchar; // a unicode code point

    template <typename Iterator>
    struct white_space : grammar<Iterator, unicode>
    {
        white_space() : white_space::base_type(start)
        {
            start =
                    space                           // tab/space/cr/lf
                |   ';' >> *(char_ - eol) >> eol    // comments
                ;
        }

        rule<Iterator, unicode> start;
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

        struct push_symbol_utf8
        {
            template <typename S, typename C>
            struct result { typedef void type; };

            void operator()(std::string& utf8, uchar code_point) const
            {
                if (utf8.size() == 0)
                    utf8 += '\0';   //  mark a symbol with prefix 0
                                    //  (a 0 byte at the beginning signifies a symbol)
                push_utf8()(utf8, code_point);
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
    struct string : grammar<Iterator, unicode, std::string()>
    {
        string() : string::base_type(start)
        {
            uint_parser<uchar, 16> hex;
            function<detail::push_utf8> push_utf8;
            function<detail::push_esc> push_esc;

            str_esc
                =  '\\'
                >>  (   ('u' >> hex)                [push_utf8(_r1, _1)]
                    |   char_("btnfr\\\"'")         [push_esc(_r1, _1)]
                    )
                ;

            start
                = '"'
                >> *(str_esc(_val) | (char_ - '"')  [push_utf8(_val, _1)])
                >> '"'
                ;
        }

        rule<Iterator, unicode, void(std::string&)> str_esc;
        rule<Iterator, unicode, std::string()> start;
    };

    template <typename Iterator>
    struct sexpr : grammar<Iterator, unicode, white_space<Iterator>, utree()>
    {
        sexpr() : sexpr::base_type(start)
        {
            real_parser<double, strict_real_policies<double> > strict_double;
            function<detail::push_symbol_utf8> push_symbol_utf8;

            start   = atom | list;

            list    = '(' >> *start >> ')';

            atom    = number                            [_val = _1]
                    | bool_                             [_val = _1]
                    | string                            [_val = _1]
                    | symbol                            [_val = _1]
                    ;

            char const* exclude = " ();\"\n\r\t";
            symbol  = +lexeme[print - char_(exclude)]   [push_symbol_utf8(_val, _1)];

            number  = strict_double                     [_val = _1]
                    | lexeme[no_case["0x"] >> hex]      [_val = _1]
                    | lexeme['0' >> oct]                [_val = _1]
                    | int_                              [_val = _1]
                    ;
        }

        rule<Iterator, unicode, white_space<Iterator>, utree()> start, list;
        rule<Iterator, unicode, utree()> atom, number;
        rule<Iterator, unicode, std::string()> symbol;
        scheme::string<Iterator> string;
    };
}

#endif
