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
    using boost::spirit::qi::parse;

    std::string s(input);
    std::string::iterator f(s.begin());
    if (parse(f, s.end(), p) && f == s.end())
        std::cout << "ok" << std::endl;
    else
        std::cout << "fail" << std::endl;
}

template <typename P>
void test_phrase_parser(char const* input, P const& p)
{
    using boost::spirit::qi::phrase_parse;
    using boost::spirit::qi::ascii::space;
    
    std::string s(input);
    std::string::iterator f(s.begin());
    if (phrase_parse(f, s.end(), p, space) && f == s.end())
        std::cout << "ok" << std::endl;
    else
        std::cout << "fail" << std::endl;
}
//]

//[reference_test_attr
template <typename P, typename T>
void test_parser_attr(char const* input, P const& p, T& attr)
{
    using boost::spirit::qi::parse;

    std::string s(input);
    std::string::iterator f(s.begin());
    if (parse(f, s.end(), p, attr) && f == s.end())
        std::cout << "ok" << std::endl;
    else
        std::cout << "fail" << std::endl;
}

template <typename P, typename T>
void test_phrase_parser_attr(char const* input, P const& p, T& attr)
{
    using boost::spirit::qi::phrase_parse;
    using boost::spirit::qi::ascii::space;

    std::string s(input);
    std::string::iterator f(s.begin());
    if (phrase_parse(f, s.end(), p, space, attr) && f == s.end())
        std::cout << "ok" << std::endl;
    else
        std::cout << "fail" << std::endl;
}
//]

//[reference_test_real_policy
///////////////////////////////////////////////////////////////////////////////
//  These policies can be used to parse thousand separated
//  numbers with at most 2 decimal digits after the decimal
//  point. e.g. 123,456,789.01
///////////////////////////////////////////////////////////////////////////////
template <typename T>
struct ts_real_policies : boost::spirit::qi::ureal_policies<T>
{
    //  2 decimal places Max
    template <typename Iterator, typename Attribute>
    static bool
    parse_frac_n(Iterator& first, Iterator const& last, Attribute& attr)
    {
        return boost::spirit::qi::
            extract_uint<T, 10, 1, 2, true>::call(first, last, attr);
    }

    //  No exponent
    template <typename Iterator>
    static bool
    parse_exp(Iterator&, Iterator const&)
    {
        return false;
    }

    //  No exponent
    template <typename Iterator, typename Attribute>
    static bool
    parse_exp_n(Iterator&, Iterator const&, Attribute&)
    {
        return false;
    }

    //  Thousands separated numbers
    template <typename Iterator, typename Attribute>
    static bool
    parse_n(Iterator& first, Iterator const& last, Attribute& attr)
    {
        using boost::spirit::qi::uint_parser;
        namespace qi = boost::spirit::qi;

        uint_parser<unsigned, 10, 1, 3> uint3;
        uint_parser<unsigned, 10, 3, 3> uint3_3;

        T result = 0;
        if (parse(first, last, uint3, result))
        {
            bool hit = false;
            T n;
            Iterator save = first;

            while (qi::parse(first, last, ',') && qi::parse(first, last, uint3_3, n))
            {
                result = result * 1000 + n;
                save = first;
                hit = true;
            }

            first = save;
            if (hit)
                attr = result;
            return hit;
        }
        return false;
    }
};
//]

int
main()
{
    {
        //[reference_using_declarations_lit_char
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
        //[reference_using_declarations_lexeme
        using boost::spirit::qi::lexeme;
        using boost::spirit::qi::lit;
        using boost::spirit::ascii::digit;
        //]
        
        //[reference_lexeme
        /*`The use of lexeme here will prevent skipping in between the
            digits and the sign making inputs such as `"1 2 345"` erroneous.*/
        test_phrase_parser("12345", lexeme[ -(lit('+') | '-') >> +digit ]);
        //]
    }
    
    {
        //[reference_using_declarations_no_case
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
    
    {
        //[reference_using_declarations_omit
        using boost::spirit::qi::omit;
        using boost::spirit::qi::int_;
        using boost::spirit::ascii::char_;
        //]
        
        //[reference_omit
        /*`This parser ignores the first two characters
            and extracts the succeeding `int`:*/
        int i;
        test_parser_attr("xx345", omit[char_ >> char_] >> int_, i);
        std::cout << i << std::endl; // should print 345
        //]
    }
    
    {
        //[reference_using_declarations_raw
        using boost::spirit::qi::raw;
        using boost::spirit::ascii::alpha;
        using boost::spirit::ascii::alnum;
        //]
        
        //[reference_raw
        //`This parser matches and extracts C++ identifiers:
        std::string id;
        test_parser_attr("James007", raw[(alpha | '_') >> *(alnum | '_')], id);
        std::cout << id << std::endl; // should print James007
        //]
    }
    
    {
        //[reference_using_declarations_repeat
        using boost::spirit::qi::repeat;
        using boost::spirit::qi::lit;
        using boost::spirit::qi::uint_parser;
        using boost::spirit::qi::_1;
        using boost::spirit::ascii::char_;
        namespace phx = boost::phoenix;
        //]
        
        //[reference_repeat
        //`A parser for a file name with a maximum of 255 characters:
        test_parser("batman.jpeg", repeat(1, 255)[char_("a-zA-Z_./")]);
        
        /*`A parser for a specific bitmap file format which has exactly 4096 RGB color information.
            (for the purpose of this example, we will be testing only 3 RGB color information.)
         */
        uint_parser<unsigned, 16, 6, 6> rgb;
        std::vector<unsigned> colors;
        test_parser_attr("ffffff0000003f3f3f", repeat(3)[rgb], colors);
        std::cout 
            << std::hex
            << colors[0] << ',' 
            << colors[1] << ',' 
            << colors[2] << std::endl;
        
        /*`A 256 bit binary string (1..256 1s or 0s). (For the purpose of this example, 
            we will be testing only 16 bits.)
         */
        test_parser("1011101011110010", repeat(16)[lit('1') | '0']);
        //]
        
        std::cout << std::dec; // reset to decimal
        
        //[reference_repeat_pascal
        /*`This trivial example cannot be practically defined in traditional EBNF. 
            Although some EBNF variants allow more powerful repetition constructs other 
            than the Kleene Star, we are still limited to parsing fixed strings. 
            The nature of EBNF forces the repetition factor to be a constant. 
            On the other hand, Spirit allows the repetition factor to be variable at 
            run time. We could write a grammar that accepts the input string above. 
            Example using phoenix: 
         */
        std::string str;
        int n;
        test_parser_attr("\x0bHello World", 
            char_[phx::ref(n) = _1] >> repeat(phx::ref(n))[char_], str);
        std::cout << n << ',' << str << std::endl;  // will print "11,Hello World"
        //]
    }
    
    {
        //[reference_using_declarations_skip
        using boost::spirit::qi::skip;
        using boost::spirit::qi::int_;
        using boost::spirit::ascii::space;
        //]
        
        //[reference_skip
        /*`Explicitly specify a skip parser. This parser parses comma 
            delimited numbers, ignoring spaces.*/
        test_parser("1, 2, 3, 4, 5", skip(space)[int_ >> *(',' >> int_)]);
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
    
    // eps
    {
        //[reference_using_declarations_eps
        using boost::spirit::qi::eps;
        using boost::spirit::qi::int_;
        using boost::spirit::qi::_1;
        namespace phx = boost::phoenix;
        //]

        //[reference_eps
        //`Basic `eps`:
        test_parser("", eps); // always matches
        //]
        
        //[reference_eps_if
        /*`This example simulates the "classic" `if_p` parser. Here, `int_` will be 
            tried only if the condition, `c`, is true.
         */
        bool c = true; // a flag
        test_parser("1234", eps(phx::ref(c) == true) >> int_);
        //]
        
        //[reference_eps_while
        /*`This example simulates the "classic" `while_p` parser. Here, the kleene loop
            will exit once the condition, c, becomes true. Notice that the condition, `c,
            is turned to `false` when we get to parse `4`.
         */
        test_phrase_parser("1 2 3 4", 
            *(eps(phx::ref(c) == true) >> int_[phx::ref(c) = (_1 == 4)]));
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
    
    // uint
    {
        //[reference_using_declarations_uint
        using boost::spirit::qi::uint_;
        using boost::spirit::qi::uint_parser;
        //]

        //[reference_uint
        test_parser("12345", uint_);
        //]
        
        //[reference_thousand_separated
        //`Thousand separated number parser:
        uint_parser<unsigned, 10, 1, 3> uint3_p;        //  1..3 digits
        uint_parser<unsigned, 10, 3, 3> uint3_3_p;      //  exactly 3 digits        
        test_parser("12,345,678", uint3_p >> *(',' >> uint3_3_p));
        //]
    }
    
    // int
    {
        //[reference_using_declarations_int
        using boost::spirit::qi::int_;
        //]

        //[reference_int
        test_parser("+12345", int_);
        test_parser("-12345", int_);
        //]
    }
    
    // real
    {
        //[reference_using_declarations_real
        using boost::spirit::qi::double_;
        using boost::spirit::qi::real_parser;
        //]

        //[reference_real
        test_parser("+12345e6", double_);
        //]
        
        //[reference_custom_real
        real_parser<double, ts_real_policies<double> > ts_real;
        test_parser("123,456,789.01", ts_real);
        //]
    }
    
    // sequence
    {
        //[reference_using_declarations_sequence
        using boost::spirit::ascii::char_;
        using boost::spirit::qi::_1;
        using boost::spirit::qi::_2;
        namespace bf = boost::fusion;
        //]
        
        //[reference_sequence
        //`Simple usage:
        test_parser("xy", char_ >> char_);
        
        //`Extracting the attribute tuple (using __fusion__):
        bf::vector<char, char> attr;
        test_parser_attr("xy", char_ >> char_, attr);
        std::cout << bf::at_c<0>(attr) << ',' << bf::at_c<1>(attr) << std::endl;
        
        //`Extracting the attribute vector (using __stl__):
        std::vector<char> vec;
        test_parser_attr("xy", char_ >> char_, vec);
        std::cout << vec[0] << ',' << vec[1] << std::endl;
        
        //`Extracting the attributes using __qi_semantic_actions__ (using __phoenix__):
        test_parser("xy", (char_ >> char_)[std::cout << _1 << ',' << _2 << std::endl]);
        //]

    }

    return 0;
}
