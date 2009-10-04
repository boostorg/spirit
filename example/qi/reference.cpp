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
#include <cstdlib>
//]

//[reference_test
template <typename P>
void test_parser(
    char const* input, P const& p, bool full_match = true)
{
    using boost::spirit::qi::parse;

    char const* f(input);
    char const* l(f + strlen(f));
    if (parse(f, l, p) && (!full_match || (f == l)))
        std::cout << "ok" << std::endl;
    else
        std::cout << "fail" << std::endl;
}

template <typename P>
void test_phrase_parser(
    char const* input, P const& p, bool full_match = true)
{
    using boost::spirit::qi::phrase_parse;
    using boost::spirit::qi::ascii::space;
    
    char const* f(input);
    char const* l(f + strlen(f));
    if (phrase_parse(f, l, p, space) && (!full_match || (f == l)))
        std::cout << "ok" << std::endl;
    else
        std::cout << "fail" << std::endl;
}
//]

//[reference_test_attr
template <typename P, typename T>
void test_parser_attr(
    char const* input, P const& p, T& attr, bool full_match = true)
{
    using boost::spirit::qi::parse;

    char const* f(input);
    char const* l(f + strlen(f));
    if (parse(f, l, p, attr) && (!full_match || (f == l)))
        std::cout << "ok" << std::endl;
    else
        std::cout << "fail" << std::endl;
}

template <typename P, typename T>
void test_phrase_parser_attr(
    char const* input, P const& p, T& attr, bool full_match = true)
{
    using boost::spirit::qi::phrase_parse;
    using boost::spirit::qi::ascii::space;

    char const* f(input);
    char const* l(f + strlen(f));
    if (phrase_parse(f, l, p, space, attr) && (!full_match || (f == l)))
        std::cout << "ok" << std::endl;
    else
        std::cout << "fail" << std::endl;
}
//]

//[reference_print_info
struct printer
{
    typedef boost::spirit::utf8_string string;

    void element(string const& tag, string const& value, int depth) const
    {
        for (int i = 0; i < (depth*4); ++i) // indent to depth
            std::cout << ' ';

        std::cout << "tag: " << tag;
        if (value != "")
            std::cout << ", value: " << value;
        std::cout << std::endl;
    }
};

void print_info(boost::spirit::info const& what)
{
    using boost::spirit::basic_info_walker;

    printer pr;
    basic_info_walker<printer> walker(pr, what.tag, 0);
    boost::apply_visitor(walker, what.value);
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

//[reference_test_bool_policy
///////////////////////////////////////////////////////////////////////////////
//  These policies can be used to parse "eurt" (i.e. "true" spelled backwards) 
//  as `false`
///////////////////////////////////////////////////////////////////////////////
struct backwards_bool_policies : boost::spirit::qi::bool_policies<>
{
    // we want to interpret a 'true' spelled backwards as 'false'
    template <typename Iterator, typename Attribute>
    static bool
    parse_false(Iterator& first, Iterator const& last, Attribute& attr)
    {
        namespace qi = boost::spirit::qi;
        if (qi::detail::string_parse("eurt", first, last, qi::unused))
        {
            spirit::traits::assign_to(false, attr);    // result is false
            return true;
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

    // bool_
    {
        //[reference_using_declarations_bool
        using boost::spirit::qi::bool_;
        using boost::spirit::qi::bool_parser;
        //]

        //[reference_bool
        test_parser("true", bool_);
        test_parser("false", bool_);
        //]

        //[reference_custom_bool
        bool_parser<bool, backwards_bool_policies> backwards_bool;
        test_parser("true", backwards_bool);
        test_parser("eurt", backwards_bool);
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
    
    // sequential_or
    {
        //[reference_using_declarations_sequential_or
        using boost::spirit::qi::int_;
        //]
        
        //[reference_sequential_or
        //`Correctly parsing a number with optional fractional digits:
        test_parser("123.456", int_ || ('.' >> int_));  // full
        test_parser("123", int_ || ('.' >> int_));      // just the whole number
        test_parser(".456", int_ || ('.' >> int_));     // just the fraction

        /*`A naive but incorrect solution would try to do this using optionals (e.g.):
                
                int_ >> -('.' >> int_)  // will not match ".456"
                -int_ >> ('.' >> int_)  // will not match "123"
                -int_ >> -('.' >> int_) // will match empty strings! Ooops.
         */
        //]
    }
    
    // alternative
    {
        //[reference_using_declarations_alternative
        using boost::spirit::ascii::string;
        using boost::spirit::qi::int_;
        using boost::spirit::qi::_1;
        using boost::variant;
        //]
        
        //[reference_alternative
        //`Simple usage:
        test_parser("Hello", string("Hello") | int_);
        test_parser("123", string("Hello") | int_);

        //`Extracting the attribute variant (using __boost_variant__):
        variant<std::string, int> attr;
        test_parser_attr("Hello", string("Hello") | int_, attr);
        
        /*`This should print `"Hello"`. Note: There are better ways to extract the value
            from the variant. See __boost_variant__ visitation. This code is solely
            for demonstration.
         */
        if (boost::get<int>(&attr))
            std::cout << boost::get<int>(attr) << std::endl;
        else
            std::cout << boost::get<std::string>(attr) << std::endl;

        /*`Extracting the attributes using __qi_semantic_actions__ with __phoenix__
            (this should print `123`):
         */
        test_parser("123", (string("Hello") | int_)[std::cout << _1 << std::endl]);
        //]

    }
    
    // permutation
    {
        //[reference_using_declarations_permutation
        using boost::spirit::ascii::char_;
        //]
        
        //[reference_permutation
        //`Parse a string containing DNA codes (ACTG)
        test_parser("ACTGGCTAGACT", *(char_('A') ^ 'C' ^ 'T' ^ 'G'));
        //]
    }
    
    // expect
    {
        //[reference_using_declarations_expect
        using boost::spirit::ascii::char_;
        using boost::spirit::qi::expectation_failure;
        //]

        //[reference_expect
        /*`The code below uses an expectation operator to throw an __qi_expectation_failure__
            with a deliberate parsing error when `"o"` is expected and `"i"` is what is
            found in the input. The `catch` block prints the information related to the
            error. Note: This is low level code that demonstrates the /bare-metal/. Typically,
            you use an __qi_error_handler__ to deal with the error.
         */
        try
        {
            test_parser("xi", char_('x') > char_('o')); // should throw an exception
        }
        catch (expectation_failure<char const*> const& x)
        {
            std::cout << "expected: "; print_info(x.what);
            std::cout << "got: \"" << std::string(x.first, x.last) << '"' << std::endl;
        }
        /*`The code above will print:[teletype]
        
                expected: tag: literal-char, value: o
                got: "i"``[c++]``
         */
        //]
    }

    // and-predicate
    {
        //[reference_and_predicate
        //`Some using declarations:
        using boost::spirit::lit;
        
        /*`Basic look-ahead example: make sure that the last character is a
            semicolon, but don't consume it, just peek at the next character:
         */
        test_phrase_parser("Hello ;", lit("Hello") >> &lit(';'), false);
        //]
    }
    
    // not-predicate
    {
        //[reference_not_predicate
        //`Some using declarations:
        using boost::spirit::ascii::char_;
        using boost::spirit::ascii::alpha;
        using boost::spirit::qi::lit;
        using boost::spirit::qi::symbols;
        
        /*`Here's an alternative to the `*(r - x) >> x` idiom using the
            not-predicate instead. This parses a list of characters terminated
            by a ';':
         */
        test_parser("abcdef;", *(!lit(';') >> char_) >> ';');
        
        /*`The following parser ensures that we match distinct keywords 
            (stored in a symbol table). To do this, we make sure that the  
            keyword does not follow an alpha or an underscore:
         */
        symbols<char, int> keywords;
        keywords = "begin", "end", "for";
        
        // This should fail:
        test_parser("beginner", keywords >> !(alpha | '_'));

        // This is ok:
        test_parser("end ", keywords >> !(alpha | '_'), false);

        // This is ok:
        test_parser("for()", keywords >> !(alpha | '_'), false);
        //]
    }
    
    // difference
    {
        //[reference_difference
        //`Some using declarations:
        using boost::spirit::ascii::char_;
        
        /*`Parse a C/C++ style comment:
         */
        test_parser("/*A Comment*/", "/*" >> *(char_ - "*/") >> "*/");
        //]
    }
    
    // kleene
    {
        //[reference_kleene
        //`Some using declarations:
        using boost::spirit::qi::int_;
        
        /*`Parse a comma separated list of numbers and put them in a vector:
         */
        std::vector<int> attr;
        test_phrase_parser_attr(
            "111, 222, 333, 444, 555", int_ >> *(',' >> int_), attr);
        std::cout 
            << attr[0] << ',' << attr[1] << ',' << attr[2] << ',' 
            << attr[3] << ',' << attr[4] 
            << std::endl;
        //]
    }
    
    // plus
    {
        //[reference_plus
        //`Some using declarations:
        using boost::spirit::ascii::alpha;
        using boost::spirit::qi::lexeme;

        /*`Parse one or more strings containing one or more alphabetic 
            characters and put them in a vector:
         */
        std::vector<std::string> attr;
        test_phrase_parser_attr("yaba daba doo", +lexeme[+alpha], attr);
        std::cout << attr[0] << ',' << attr[1] << ',' << attr[2] << std::endl;
        //]
    }
    
    // optional
    {
        //[reference_optional
        //`Some using declarations:
        using boost::spirit::ascii::char_;
        using boost::spirit::qi::lexeme;
        using boost::spirit::qi::int_;
        using boost::fusion::vector;
        using boost::fusion::at_c;
        using boost::optional;

        /*`Parse a person info with name (in quotes) optional age [footnote 
            James Bond is shy about his age :-)] and optional sex, all 
            separated by comma.
         */
        vector<std::string, optional<int>, optional<char> > attr;
        
        test_phrase_parser_attr(
            "\"James Bond\", M"
          , lexeme['"' >> +(char_ - '"') >> '"']    // name
                >> -(',' >> int_)                   // optional age
                >> -(',' >> char_)                  // optional sex
          , attr);
        
        // Should print: James Bond,M
        std::cout << at_c<0>(attr);                 // print name
        if (at_c<1>(attr))                          // print optional age
            std::cout << ',' << *at_c<1>(attr);
        if (at_c<2>(attr))                          // print optional sex
            std::cout << ',' << *at_c<2>(attr);
        std::cout << std::endl;        
        //]
    }
    
    // list
    {
        //[reference_list
        //`Some using declarations:
        using boost::spirit::qi::int_;
        
        /*`Parse a comma separated list of numbers and put them in a vector:
         */
        std::vector<int> attr;
        test_phrase_parser_attr(
            "111, 222, 333, 444, 555", int_ % ',', attr);
        std::cout 
            << attr[0] << ',' << attr[1] << ',' << attr[2] << ',' 
            << attr[3] << ',' << attr[4] 
            << std::endl;
        //]
    }
    
    // native binary
    {
        //[reference_qi_native_binary
        //`Using declarations and variables: 
        using boost::spirit::qi::byte_;
        using boost::spirit::qi::word;
        using boost::spirit::qi::dword;
        using boost::spirit::qi::qword;
        
        boost::uint8_t uc;
        boost::uint16_t us;
        boost::uint32_t ui;
//<-
#ifdef BOOST_HAS_LONG_LONG
//->
        boost::uint64_t ul;
//<-
#endif

#ifdef BOOST_LITTLE_ENDIAN
//->
        //`Basic usage of the native binary parsers for little endian platforms: 
        test_parser_attr("\x01", byte_, uc); assert(uc == 0x01);
        test_parser_attr("\x01\x02", word, us); assert(us == 0x0201);
        test_parser_attr("\x01\x02\x03\x04", dword, ui); assert(ui == 0x04030201);
//<-
#ifdef BOOST_HAS_LONG_LONG
//->
        test_parser_attr("\x01\x02\x03\x04\x05\x06\x07\x08", qword, ul);
        assert(ul == 0x0807060504030201LL);

//<-
#endif
//->
        test_parser("\x01", byte_(0x01));
        test_parser("\x01\x02", word(0x0201));
        test_parser("\x01\x02\x03\x04", dword(0x04030201));
//<-
#ifdef BOOST_HAS_LONG_LONG
//->        
        test_parser("\x01\x02\x03\x04\x05\x06\x07\x08",
            qword(0x0807060504030201LL));
//<-
#endif
#else
//->
        //`Basic usage of the native binary parsers for big endian platforms: 
        test_parser_attr("\x01", byte_, uc); assert(uc == 0x01);
        test_parser_attr("\x01\x02", word, us); assert(us ==  0x0102);
        test_parser_attr("\x01\x02\x03\x04", dword, ui); assert(ui == 0x01020304);
//<-
#ifdef BOOST_HAS_LONG_LONG
//->        
        test_parser_attr("\x01\x02\x03\x04\x05\x06\x07\x08", qword, ul);
        assert(0x0102030405060708LL);

//<-
#endif
//->
        test_parser("\x01", byte_(0x01));
        test_parser("\x01\x02", word(0x0102));
        test_parser("\x01\x02\x03\x04", dword(0x01020304));
//<-
#ifdef BOOST_HAS_LONG_LONG
//->        
        test_parser("\x01\x02\x03\x04\x05\x06\x07\x08",
            qword(0x0102030405060708LL));
//<-
#endif
#endif
//->        
        //]
    }
    
    // little binary
    {
        //[reference_qi_little_binary
        //`Using declarations and variables: 
        using boost::spirit::qi::little_word;
        using boost::spirit::qi::little_dword;
        using boost::spirit::qi::little_qword;
        
        boost::uint8_t uc;
        boost::uint16_t us;
        boost::uint32_t ui;
//<-
#ifdef BOOST_HAS_LONG_LONG
//->
        boost::uint64_t ul;
//<-
#endif

//->
        //`Basic usage of the little endian binary parsers: 
        test_parser_attr("\x01\x02", little_word, us); assert(us == 0x0201);
        test_parser_attr("\x01\x02\x03\x04", little_dword, ui); assert(ui == 0x04030201);
//<-
#ifdef BOOST_HAS_LONG_LONG
//->
        test_parser_attr("\x01\x02\x03\x04\x05\x06\x07\x08", little_qword, ul);
        assert(ul == 0x0807060504030201LL);

//<-
#endif
//->
        test_parser("\x01\x02", little_word(0x0201));
        test_parser("\x01\x02\x03\x04", little_dword(0x04030201));
//<-
#ifdef BOOST_HAS_LONG_LONG
//->        
        test_parser("\x01\x02\x03\x04\x05\x06\x07\x08",
            little_qword(0x0807060504030201LL));
//<-
#endif
//->        
        //]
    }
    
    // big binary
    {
        //[reference_qi_big_binary
        //`Using declarations and variables: 
        using boost::spirit::qi::big_word;
        using boost::spirit::qi::big_dword;
        using boost::spirit::qi::big_qword;
        
        boost::uint8_t uc;
        boost::uint16_t us;
        boost::uint32_t ui;
//<-
#ifdef BOOST_HAS_LONG_LONG
//->
        boost::uint64_t ul;
//<-
#endif

//->
        //`Basic usage of the big endian binary parsers: 
        test_parser_attr("\x01\x02", big_word, us); assert(us ==  0x0102);
        test_parser_attr("\x01\x02\x03\x04", big_dword, ui); assert(ui == 0x01020304);
//<-
#ifdef BOOST_HAS_LONG_LONG
//->        
        test_parser_attr("\x01\x02\x03\x04\x05\x06\x07\x08", big_qword, ul);
        assert(0x0102030405060708LL);

//<-
#endif
//->
        test_parser("\x01\x02", big_word(0x0102));
        test_parser("\x01\x02\x03\x04", big_dword(0x01020304));
//<-
#ifdef BOOST_HAS_LONG_LONG
//->        
        test_parser("\x01\x02\x03\x04\x05\x06\x07\x08",
            big_qword(0x0102030405060708LL));
//<-
#endif
//->        
        //]
    }
   
    return 0;
}
