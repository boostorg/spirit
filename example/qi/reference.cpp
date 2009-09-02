/*=============================================================================
    Copyright (c) 2001-2009 Joel de Guzman
    http://spirit.sourceforge.net/

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
//[reference_includes
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <iostream>
#include <string>
//]

//[reference_test
template <typename P>
void test_parser(char const* input, P const& p)
{
    std::string s(input);
    std::string::iterator f(s.begin());
    if (boost::spirit::qi::parse(f, s.end(), p))
        std::cout << "ok" << std::endl;
    else
        std::cout << "fail" << std::endl;
}
//]

//[reference_test_attr
template <typename P, typename T>
void test_parser_attr(char const* input, P const& p, T& attr)
{
    std::string s(input);
    std::string::iterator f(s.begin());
    if (boost::spirit::qi::parse(f, s.end(), p, attr))
        std::cout << "ok" << std::endl;
    else
        std::cout << "fail" << std::endl;
}
//]

int
main()
{
    {
        //[reference_using_declarations_lit_char
        using boost::spirit::qi::parse;
        using boost::spirit::qi::lit;
        using boost::spirit::ascii::char_;
        //]
        
        //[reference_char_literals
        test_parser("x", 'x');                      // plain literal
        test_parser("x", lit('x'));                 // explicit literal
        test_parser("x", char_('x'));               // ascii::char_
        //]
        
        //[reference_char_range
        char ch;
        test_parser_attr("5", char_('0','9'), ch);  // ascii::char_ range
        std::cout << ch << std::endl;               // prints '5'
        //]
        
        //[reference_char_set
        test_parser_attr("5", char_("0-9"), ch);    // ascii::char_ set
        std::cout << ch << std::endl;               // prints '5'
        //]
        
        //[reference_char_phoenix
        namespace phx = boost::phoenix;        
        test_parser("x", phx::val('x'));            // direct
        test_parser("5", 
            char_(phx::val('0'),phx::val('9')));    // ascii::char_ range
        //]
    }
    
    {
        //[reference_using_declarations_lit_string
        using boost::spirit::qi::parse;
        using boost::spirit::qi::lit;
        using boost::spirit::ascii::string;
        //]
        
        //[reference_string_literals
        test_parser("boost", "boost");          // plain literal
        test_parser("boost", lit("boost"));     // explicit literal
        test_parser("boost", string("boost"));  // ascii::string
        //]
    }
    
    {
        using boost::spirit::qi::parse;
        using boost::spirit::qi::lit;
        using boost::spirit::ascii::string;
        
        //[reference_string_std_string
        std::string s("boost");
        test_parser("boost", s);            // direct
        test_parser("boost", lit(s));       // explicit
        test_parser("boost", string(s));    // ascii::string
        //]
    }
    
    {
        using boost::spirit::qi::parse;
        using boost::spirit::qi::lit;
        using boost::spirit::ascii::string;
        
        //[reference_string_phoenix
        namespace phx = boost::phoenix;        
        test_parser("boost", phx::val("boost"));            // direct
        test_parser("boost", lit(phx::val("boost")));       // explicit
        test_parser("boost", string(phx::val("boost")));    // ascii::string
        //]
    }
    
    {
        //[reference_using_declarations_symbols
        using boost::spirit::qi::parse;
        using boost::spirit::qi::symbols;
        //]

        //[reference_symbols_with_data
        symbols<char, int> sym;

        sym.add
            ("Apple", 1)
            ("Banana", 2)
            ("Orange", 3)
        ;

        int i;
        test_parser_attr("Banana", sym, i);
        std::cout << i << std::endl;
        //]
    }
    
    {
        //[reference_using_declarations_no_case
        using boost::spirit::qi::parse;
        using boost::spirit::ascii::no_case;
        using boost::spirit::ascii::char_;
        using boost::spirit::ascii::alnum;
        using boost::spirit::qi::symbols;
        //]
        
        //[reference_no_case
        test_parser("X", no_case[char_('x')]);
        test_parser("6", no_case[alnum]);
        //]
        
        //[reference_symbols_with_no_case
        symbols<char, int> sym;

        sym.add
            ("apple", 1)    // symbol strings are added in lowercase...
            ("banana", 2)
            ("orange", 3)
        ;

        int i;
        // ...because sym is used for case-insensitive parsing
        test_parser_attr("Apple", no_case[ sym ], i);
        std::cout << i << std::endl;
        test_parser_attr("ORANGE", no_case[ sym ], i);
        std::cout << i << std::endl;
        //]
    }

    // attr()
    {
        //[reference_using_declarations_attr
        namespace phx = boost::phoenix;
        using boost::spirit::qi::attr;
        //]

        //[reference_attr
        std::string str;
        test_parser_attr("", attr("boost"), str);
        std::cout << str << std::endl;            // will print 'boost'

        double d;
        test_parser_attr("", attr(1.0), d);
        std::cout << d << std::endl;              // will print '1.0'
        //]

        //[reference_attr_phoenix
        d = 0.0;
        double d1 = 1.2;
        test_parser_attr("", attr(phx::ref(d1)), d);
        std::cout << d << std::endl;              // will print '1.2'
        //]
    }
    
    // eol
    {
        //[reference_using_declarations_eol
        using boost::spirit::qi::eol;
        //]

        //[reference_eol
        test_parser("\n", eol);
        //]
    }
    
    // eoi
    {
        //[reference_using_declarations_eoi
        using boost::spirit::qi::eoi;
        //]

        //[reference_eoi
        test_parser("", eoi);
        //]
    }
    
    // lazy
    {
        //[reference_using_declarations_lazy
        using boost::spirit::qi::lazy;
        using boost::spirit::ascii::string;
        using boost::phoenix::val;
        //]

        //[reference_lazy
        /*` Here, the phoenix::val expression creates a function
            that returns its argument when invoked. The lazy expression
            defers the invocation of this function at parse time. Then,
            this parser (string parser) is called into action. All this
            takes place at parse time.
         */
        test_parser("Hello", lazy(val(string("Hello"))));
        
        //` The above is equivalent to:
        test_parser("Hello", val(string("Hello")));
        //]
    }
    
    // char class
    {
        //[reference_using_declarations_char_class
        using boost::spirit::ascii::alnum;
        using boost::spirit::ascii::blank;
        using boost::spirit::ascii::digit;
        using boost::spirit::ascii::lower;
        //]

        //[reference_char_class        
        test_parser("1", alnum);
        test_parser(" ", blank);
        test_parser("1", digit);
        test_parser("a", lower);
        //]
    }
    return 0;
}
