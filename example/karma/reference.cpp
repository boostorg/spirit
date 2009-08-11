/*=============================================================================
    Copyright (c) 2001-2009 Hartmut Kaiser
    http://spirit.sourceforge.net/

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
//[reference_karma_includes
#include <boost/spirit/include/karma.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/fusion/include/std_pair.hpp>
#include <iostream>
#include <string>
//]

//[reference_karma_test
template <typename G>
void test_generator(char const* expected, G const& g)
{
    std::string s;
    std::back_insert_iterator<std::string> out(s);
    if (boost::spirit::karma::generate(out, g) && str == expected)
        std::cout << "ok" << std::endl;
    else
        std::cout << "fail" << std::endl;
}
//]

//[reference_karma_test_attr
template <typename G, typename T>
void test_generator_attr(char const* expected, G const& g, T const& attr)
{
    std::string s;
    std::back_insert_iterator<std::string> out(s);
    if (boost::spirit::karma::generate(out, g, attr) && str == expected)
        std::cout << "ok" << std::endl;
    else
        std::cout << "fail" << std::endl;
}
//]

int
main()
{
    ///////////////////////////////////////////////////////////////////////////
    // Operators
    ///////////////////////////////////////////////////////////////////////////
    {
        //[reference_karma_using_declarations_sequence
        using boost::spirit::karma::generate;
        using boost::spirit::karma::double_;
        //]

        //[reference_karma_sequence
        test_generator_attr("1.0,2.0", double_ << ',' << double_, std::make_pair(1.0, 2.0));
        //]
    }

    {
        //[reference_karma_using_declarations_alternative
        using boost::spirit::karma::generate;
        using boost::spirit::karma::double_;
        using boost::spirit::karma::ascii::string;
        //]

        //[reference_karma_alternative1
        boost::variant<std::string, double> v1(1.0);
        test_generator_attr("1.0", string | double_, v1);
        test_generator_attr("2.0", string | double_, 2.0);
        //]

        //[reference_karma_alternative2
        boost::variant<std::string, double> v2("example");
        test_generator_attr("example", string | double_, v2);
        test_generator_attr("example", string | double_, "example");
        //]
    }

    {
        //[reference_karma_using_declarations_kleene
        using boost::spirit::karma::generate;
        using boost::spirit::karma::double_;
        using boost::spirit::karma::space;
        //]

        //[reference_karma_kleene
        std::vector<double> v;
        v.push_back(1.0);
        v.push_back(2.0);
        v.push_back(3.0);
        test_generator_attr("1.0 2.0 3.0 ", *double_, space, v);
        //]
    }

    {
        //[reference_karma_using_declarations_plus
        using boost::spirit::karma::generate;
        using boost::spirit::karma::double_;
        using boost::spirit::karma::space;
        //]

        //[reference_karma_plus1
        std::vector<double> v1;
        v1.push_back(1.0);
        v1.push_back(2.0);
        v1.push_back(3.0);
        test_generator_attr("1.0 2.0 3.0 ", +double_, space, v1);
        //]

        //[reference_karma_plus2
        std::vector<double> v2;               // empty container
        test_generator_attr("empty", +double_ | "empty", v2);
        //]
    }

    {
        //[reference_karma_using_declarations_list
        using boost::spirit::karma::generate;
        using boost::spirit::karma::double_;
        //]

        //[reference_karma_list
        std::vector<double> v1;
        v1.push_back(1.0);
        test_generator_attr("1.0", double_ % ',', v1);

        v1.push_back(2.0);
        test_generator_attr("1.0,2.0", double_ % ',', v1);
        //]
    }

    {
        //[reference_karma_using_declarations_optional
        using boost::spirit::karma::generate;
        using boost::spirit::karma::double_;
        //]

        //[reference_karma_optional1
        boost::optional<double> val(1.0);
        test_generator_attr("1.0", -double_, val);
        test_generator_attr("2.0", -double_, 2.0);
        //]

        //[reference_karma_optional2
        boost::optional<double> val;          // empty optional
        test_generator_attr("", -double_, val);
        //]
    }

    {
        //[reference_karma_using_declarations_and_predicate
        using boost::spirit::karma::generate;
        using boost::spirit::karma::double_;
        using boost::spirit::karma::ascii::char_;
        using boost::spirit::karma::ascii::string;
        using boost::phoenix::ref;
        //]

        //[reference_karma_and_predicate
        test_generator_attr("b", &char_('a') << 'b' | 'c', 'a');
        test_generator_attr("c", &char_('a') << 'b' | 'c', 'x');

        test_generator_attr("abc", &string("123") << "abc" | "def", "123");
        test_generator_attr("def", &string("123") << "abc" | "def", "456");
        //]
    }

    {
        //[reference_karma_using_declarations_not_predicate
        using boost::spirit::karma::generate;
        using boost::spirit::karma::double_;
        using boost::spirit::karma::ascii::char_;
        using boost::spirit::karma::ascii::string;
        using boost::phoenix::ref;
        //]

        //[reference_karma_not_predicate
        test_generator_attr("c", !char_('a') << 'b' | 'c', 'a');
        test_generator_attr("b", !char_('a') << 'b' | 'c', 'x');

        test_generator_attr("def", !string("123") << "abc" | "def", "123");
        test_generator_attr("abc", !string("123") << "abc" | "def", "456");
        //]
    }

    ///////////////////////////////////////////////////////////////////////////
    //  Directives
    ///////////////////////////////////////////////////////////////////////////
    {
        //[reference_karma_using_declarations_buffer
        using boost::spirit::karma::generate;
        using boost::spirit::karma::double_;
        using boost::spirit::karma::buffer;
        //]

        //[reference_karma_buffer
        std::vector<double> v;                // empty container
        test_generator_attr("", -buffer['[' << +double_ << ']'], v);

        v.push_back(1.0);                     // now, fill the container
        v.push_back(2.0);
        test_generator_attr("[1.02.0]", buffer['[' << +double_ << ']'], v);
        //]
    }

    {
        //[reference_karma_using_declarations_omit
        using boost::spirit::karma::generate;
        using boost::spirit::karma::double_;
        using boost::spirit::karma::buffer;
        //]

        //[reference_karma_omit
        std::pair<double, double> p (1.0, 2.0);
        test_generator_attr("2.0", omit[double_] << double_, p);
        //]
    }

    {
        //[reference_karma_using_declarations_int
        using boost::spirit::karma::generate;
        using boost::spirit::karma::int_;
        //]

        //[reference_karma_int
        test_generator("-2", lit(-2));
        test_generator("-2", int_(-2));
        test_generator_attr("-2", int_(-2), -2);
        test_generator_attr("", int_(-2), 3);    // fails (as -2 != 3)!
        test_generator_attr("-2", int_, -2);
        //]
    }

    {
        //[reference_karma_using_declarations_uint
        using boost::spirit::karma::generate;
        using boost::spirit::karma::uint;
        //]

        //[reference_karma_uint
        test_generator("2", lit(2U));
        test_generator("2", uint(2));
        test_generator_attr("2", uint(2), 2);
        test_generator_attr("", uint(2), 3);    // fails (as 2 != 3)!
        test_generator_attr("2", uint, 2);
        //]
    }

    {
        //[reference_karma_using_declarations_char_class
        using boost::spirit::karma::generate;
        using boost::spirit::karma::alpha;
        //]

        //[reference_karma_char_class
        test_generator_attr("a", alpha, 'a');
        test_generator_attr("A", alpha, 'A');
        test_generator_attr("", alpha, '1');          // fails (as isalpha('1') is false)
        test_generator_attr("A", upper[alpha], 'a');
        test_generator_attr("", upper[alpha], 'a');   // fails (as isupper('a') is false)
        //]
    }

    return 0;
}
