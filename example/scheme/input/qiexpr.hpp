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
#include <boost/spirit/include/phoenix_statement.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

#include "../utree.hpp"
#include "../utree_operators.hpp"
#include "string.hpp"

///////////////////////////////////////////////////////////////////////////////
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
    using boost::phoenix::function;

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
        }

        rule<Iterator> start;
    };

    namespace detail
    {
        ///////////////////////////////////////////////////////////////////////
        // return true if the utree instance represents a list whose first 
        // element is a symbol node equal to the second argument
        inline bool is_list_node(utree const& u, utf8_symbol const& symbol)
        {
            if (u.which() != utree_type::list_type)
                return false;
            return u.front() == symbol;
        }

        ///////////////////////////////////////////////////////////////////////
        // ensure the given utree instance represents a list whose first 
        // element is the symbol this function object has been constructed from
        struct make_list_node
        {
            template <typename S, typename C>
            struct result { typedef void type; };

            explicit make_list_node(char const* symbol_)
              : symbol(symbol_)
            {}

            void operator()(utree& seq, utree const& element) const
            {
                if (!is_list_node(seq, symbol)) {
                    utree u;
                    u.push_back(symbol);
                    u.push_back(seq);
                    seq = u;
                }
                seq.push_back(element);
            }

            utf8_symbol symbol;
        };
    }

    ///////////////////////////////////////////////////////////////////////////
    // a list of names for all supported parser primitives taking no parameters
    static char const* const primitives0[] = 
    {
      // character parsers
        "char_"
      , "alnum", "alpha", "blank", "cntrl", "digit", "graph", "print", "punct"
      , "space", "xdigit"
      , "lower", "upper"

      // numerics
      , "long_long", "long_", "int_", "short_"
      , "ulong_long", "ulong_", "uint_", "ushort_"
      , "bin", "oct", "hex"
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
            typedef function<detail::make_list_node> make_list_type;

            make_list_type make_sequence = detail::make_list_node(">>");
            make_list_type make_alternative = detail::make_list_node("|");

            start = -alternative;

            // A | B
            alternative =
                    sequence          [ _val = _1 ]
                >> *( '|' >> sequence [ make_alternative(_val, _1) ] )
                ;

            // A >> B
            sequence = 
                    term              [ _val = _1 ]
                >> *( ">>" >> term    [ make_sequence(_val, _1) ] )
                ;

            // A, (A)
            term =
                    primitive
                |   '(' >> alternative >> ')'
                ;

            // any primitive parser
            primitive =
                    primitive2 >> '(' >> literal >> ',' >> literal >> ')'
                |   primitive1 >> '(' >> literal >> ')'
                |   primitive0        // taking no parameter
                ;

            // a literal (either 'x' or "abc")
            literal = 
                    string_lit            [ push_back(_val, _1) ]
                |   string_lit.char_lit   [ push_back(_val, _1) ]
                ;

            // fill the symbol tables with all known primitive parser names
            for (char const* const* p = primitives0; *p; ++p)
            {
                utree u;
                u.push_back(utf8_symbol(*p));
                primitive0.add(*p, u);
            }

            for (char const* const* p = primitives1; *p; ++p)
            {
                utree u;
                u.push_back(utf8_symbol(*p));
                primitive1.add(*p, u);
            }

            for (char const* const* p = primitives2; *p; ++p)
            {
                utree u;
                u.push_back(utf8_symbol(*p));
                primitive2.add(*p, u);
            }
        }

        typedef rule<Iterator, qiexpr_white_space<Iterator>, utree()> rule_type;

        rule_type start, alternative, sequence, primitive, term, literal;
        symbols<char_type, utree> primitive0, primitive1, primitive2;
        scheme::input::string<Iterator> string_lit;
    };
}}

#endif
