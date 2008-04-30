/*=============================================================================
    Copyright (c) 2001-2008 Joel de Guzman
    Copyright (c) 2001-2008 Hartmut Kaiser

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
///////////////////////////////////////////////////////////////////////////////
//
//  A Calculator example demonstrating generation of AST which gets dumped into
//  a human readable format afterwards.
//
//  [ JDG April 28, 2008 ]
//  [ HK April 28, 2008 ]
//
///////////////////////////////////////////////////////////////////////////////
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/karma.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_function.hpp>
#include <boost/spirit/include/phoenix_statement.hpp>

#include <iostream>
#include <vector>
#include <string>

#include <boost/function_output_iterator.hpp>

using namespace boost::spirit;
using namespace boost::spirit::ascii;
using namespace boost::spirit::arg_names;

///////////////////////////////////////////////////////////////////////////////
//  Our AST
///////////////////////////////////////////////////////////////////////////////
struct binary_op;
struct unary_op;
struct nil {};

struct expression_ast
{
    typedef
        boost::variant<
            nil // can't happen!
          , int
          , boost::recursive_wrapper<binary_op>
          , boost::recursive_wrapper<unary_op>
        >
    type;

    // expose variant types 
    typedef type::types types;
    
    // expose variant functionality
    int which() const { return expr.which(); }
    
    // constructors
    expression_ast()
      : expr(nil()) {}

    expression_ast(unary_op const& expr)
      : expr(expr) {}

    expression_ast(binary_op const& expr)
      : expr(expr) {}

    expression_ast(unsigned int expr)
      : expr(expr) {}
      
    expression_ast(type const& expr)
      : expr(expr) {}
      
    expression_ast& operator+=(expression_ast const& rhs);
    expression_ast& operator-=(expression_ast const& rhs);
    expression_ast& operator*=(expression_ast const& rhs);
    expression_ast& operator/=(expression_ast const& rhs);

    type expr;
};

// expose variant functionality
template <typename T>
inline T get(expression_ast const& expr)
{
    return boost::get<T>(expr.expr);
}

///////////////////////////////////////////////////////////////////////////////
struct binary_op
{
    binary_op() {}
    
    binary_op(
        char op
      , expression_ast const& left
      , expression_ast const& right)
      : op(op), left(left), right(right) {}

    char op;
    expression_ast left;
    expression_ast right;
};

struct unary_op
{
    unary_op(
        char op
      , expression_ast const& right)
    : op(op), right(right) {}

    char op;
    expression_ast right;
};

expression_ast& expression_ast::operator+=(expression_ast const& rhs)
{
    expr = binary_op('+', expr, rhs);
    return *this;
}

expression_ast& expression_ast::operator-=(expression_ast const& rhs)
{
    expr = binary_op('-', expr, rhs);
    return *this;
}

expression_ast& expression_ast::operator*=(expression_ast const& rhs)
{
    expr = binary_op('*', expr, rhs);
    return *this;
}

expression_ast& expression_ast::operator/=(expression_ast const& rhs)
{
    expr = binary_op('/', expr, rhs);
    return *this;
}

// We should be using expression_ast::operator-. There's a bug
// in phoenix type deduction mechanism that prevents us from
// doing so. Phoenix will be switching to BOOST_TYPEOF. In the
// meantime, we will use a phoenix::function below:
struct negate_expr
{
    template <typename T>
    struct result { typedef T type; };

    expression_ast operator()(expression_ast const& expr) const
    {
        return unary_op('-', expr);
    }
};

boost::phoenix::function<negate_expr> neg;

///////////////////////////////////////////////////////////////////////////////
//  Our calculator parser grammar
///////////////////////////////////////////////////////////////////////////////
template <typename Iterator>
struct calculator : qi::grammar_def<Iterator, expression_ast(), space_type>
{
    calculator()
    {
        expression =
            term                            [_val = _1]
            >> *(   ('+' >> term            [_val += _1])
                |   ('-' >> term            [_val -= _1])
                )
            ;

        term =
            factor                          [_val = _1]
            >> *(   ('*' >> factor          [_val *= _1])
                |   ('/' >> factor          [_val /= _1])
                )
            ;

        factor =
            uint_                           [_val = _1]
            |   '(' >> expression           [_val = _1] >> ')'
            |   ('-' >> factor              [_val = neg(_1)])
            |   ('+' >> factor              [_val = _1])
            ;
    }

    qi::rule<Iterator, expression_ast(), space_type> expression, term, factor;
};

///////////////////////////////////////////////////////////////////////////////
//  A couple of phoenix functions helping to access the elements of the 
//  generated AST
///////////////////////////////////////////////////////////////////////////////
template <typename T>
struct get_element
{
    template <typename T1>
    struct result { typedef T const& type; };

    T const& operator()(expression_ast const& expr) const
    {
        return boost::get<T>(expr.expr);
    }
};

boost::phoenix::function<get_element<int> > _int;
boost::phoenix::function<get_element<binary_op> > _bin_op;
boost::phoenix::function<get_element<unary_op> > _unary_op;

///////////////////////////////////////////////////////////////////////////////
struct get_left
{
    template <typename T1>
    struct result { typedef expression_ast const& type; };

    expression_ast const& operator()(binary_op const& bin_op) const
    {
        return bin_op.left;
    }
};

boost::phoenix::function<get_left> _left;

struct get_right
{
    template <typename T1>
    struct result { typedef expression_ast const& type; };

    template <typename Node>
    expression_ast const& operator()(Node const& op) const
    {
        return op.right;
    }
};

boost::phoenix::function<get_right> _right;

struct get_op
{
    template <typename T1>
    struct result { typedef char type; };

    template <typename Node>
    char operator()(Node const& bin_op) const
    {
        return bin_op.op;
    }
};

boost::phoenix::function<get_op> _op;

///////////////////////////////////////////////////////////////////////////////
//  Our AST grammar for the generator
///////////////////////////////////////////////////////////////////////////////
template <typename OuputIterator>
struct dump_ast
  : karma::grammar_def<OuputIterator, expression_ast(), space_type>
{
    dump_ast()
    {
        ast_node %= 
                int_        [_1 = _int(_r0)]
            |   binary_node [_1 = _bin_op(_r0)]
            |   unary_node  [_1 = _unary_op(_r0)]
            ;
            
        binary_node = 
                ('(' << ast_node << char_ << ast_node << ')')
                [ 
                    _1 = _left(_r0), _2 = _op(_r0), _3 = _right(_r0)
                ]
            ;

        unary_node =
                ('(' << char_ << ast_node << ')')
                [
                    _1 = _op(_r0), _2 = _right(_r0)
                ]
            ;
    }

    karma::rule<OuputIterator, expression_ast(), space_type> ast_node;
    karma::rule<OuputIterator, binary_op(), space_type> binary_node;
    karma::rule<OuputIterator, unary_op(), space_type> unary_node;
};

///////////////////////////////////////////////////////////////////////////
template <typename String>
struct string_appender
{
    string_appender(String& s)
    :   str(s)
    {}

    template <typename T>
    void operator()(T const &x) const
    {
        str += x;
    }

    String& str;
};

template <typename String>
inline string_appender<String>
make_string_appender(String& str)
{
    return string_appender<String>(str);
}

///////////////////////////////////////////////////////////////////////////////
//  Main program
///////////////////////////////////////////////////////////////////////////////
int
main()
{
    using boost::make_function_output_iterator;

    std::cout << "/////////////////////////////////////////////////////////\n\n";
    std::cout << "Expression parser...\n\n";
    std::cout << "/////////////////////////////////////////////////////////\n\n";
    std::cout << "Type an expression...or [q or Q] to quit\n\n";

    //  Our parser grammar definitions
    typedef std::string::const_iterator iterator_type;
    typedef calculator<iterator_type> calculator;

    calculator def; 
    qi::grammar<calculator> calc(def, def.expression);

    // Our generator grammar definitions
    typedef 
        boost::function_output_iterator<string_appender<std::string> > 
    output_iterator_type;
    typedef dump_ast<output_iterator_type> dump_ast;
    
    dump_ast dump_ast_def;
    karma::grammar<dump_ast> ast_grammar(dump_ast_def, dump_ast_def.ast_node); 

    std::string str;
    while (std::getline(std::cin, str))
    {
        if (str.empty() || str[0] == 'q' || str[0] == 'Q')
            break;

        expression_ast ast;
        std::string::const_iterator iter = str.begin();
        std::string::const_iterator end = str.end();
        bool r = qi::phrase_parse(iter, end, calc, ast, space);

        if (r && iter == end)
        {
            std::string generated;
            r = karma::generate_delimited(
                make_function_output_iterator(make_string_appender(generated)),
                ast_grammar, ast, space
            );
            
            if (r)
            {
                std::cout << "AST for '" << str << "': " << generated 
                          << std::endl;
            }
            else
            {
                std::cout << "-------------------------\n";
                std::cout << "Generating failed\n";
                std::cout << "-------------------------\n";
            }
        }
        else
        {
            std::string rest(iter, end);
            std::cout << "-------------------------\n";
            std::cout << "Parsing failed\n";
            std::cout << "stopped at: \": " << rest << "\"\n";
            std::cout << "-------------------------\n";
        }
    }

    std::cout << "Bye... :-) \n\n";
    return 0;
}


