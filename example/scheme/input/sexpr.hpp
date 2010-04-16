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

#include <utree/utree.hpp>
#include <utree/operators.hpp>
#include <input/string.hpp>

namespace scheme { namespace input
{
    using boost::spirit::standard::char_;
    using boost::spirit::standard::space;
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
    using boost::spirit::qi::on_error;
    using boost::spirit::qi::fail;
    using boost::spirit::qi::_1;
    using boost::spirit::qi::_2;
    using boost::spirit::qi::_3;
    using boost::spirit::qi::_4;
    using boost::spirit::info;
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
        typedef
            boost::function<void(Iterator, Iterator, Iterator, info const&)>
        errorf_type;

        sexpr(errorf_type errorf = errorf_type())
          : sexpr::base_type(start), error_handler(errorf)
        {
            real_parser<double, strict_real_policies<double> > strict_double;
            uint_parser<unsigned char, 16, 2, 2> hex2;

            start   = atom | list;

            list    = '(' > *start > ')';

            atom    = strict_double
                    | integer
                    | bool_
                    | string
                    | byte_str
                    | symbol
                    ;

            std::string exclude = std::string(" ();\"\x01-\x1f\x7f") + '\0';
            symbol  = lexeme[+(~char_(exclude))];

            integer = lexeme[no_case["0x"] > hex]
                    | lexeme['0' >> oct]
                    | int_
                    ;

            byte_str = lexeme[no_case['b'] > +hex2];

            start.name("sexpr");
            list.name("list");
            atom.name("atom");
            symbol.name("symbol");
            integer.name("integer");
            byte_str.name("byte_str");
            on_error<fail>(start, error_handler(_1, _2, _3, _4));
        }

        rule<Iterator, sexpr_white_space<Iterator>, utree()> start, list;
        rule<Iterator, int()> integer;
        rule<Iterator, utree()> atom;
        rule<Iterator, utf8_symbol()> symbol;
        rule<Iterator, binary_string()> byte_str;
        scheme::input::string<Iterator> string;

        struct error_handler_
        {
            errorf_type errorf;
            error_handler_(errorf_type errorf)
              : errorf(errorf) {}

            template <typename, typename, typename, typename>
            struct result { typedef void type; };

            void operator()(
                Iterator first, Iterator last,
                Iterator err_pos, info const& what) const
            {
                if (!errorf.empty())
                {
                    errorf(first, last, err_pos, what);
                }
                else
                {
                    std::cout
                        << "Error! Expecting "
                        << what                         // what failed?
                        << " here: \""
                        << std::string(err_pos, last)   // iterators to error-pos, end
                        << "\""
                        << std::endl
                    ;
                }
            }
        };

        function<error_handler_> const error_handler;
    };
}}

#endif
