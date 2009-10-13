/*=============================================================================
    Copyright (c) 2001-2009 Hartmut Kaiser
    http://spirit.sourceforge.net/

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include <boost/config/warning_disable.hpp>

//[reference_karma_includes
#include <boost/spirit/include/karma.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/fusion/include/std_pair.hpp>
#include <iostream>
#include <string>
//]

//[reference_karma_includes_simple
#include <boost/spirit/include/karma.hpp>
#include <iostream>
#include <string>
//]

//[reference_karma_test
template <typename G>
void test_generator(char const* expected, G const& g)
{
    std::string s;
    std::back_insert_iterator<std::string> out(s);
    if (boost::spirit::karma::generate(out, g) && s == expected)
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
    if (boost::spirit::karma::generate(out, g, attr) && s == expected)
        std::cout << "ok" << std::endl;
    else
        std::cout << "fail" << std::endl;
}
//]

//[reference_karma_test_attr2
template <typename G, typename T1, typename T2>
void test_generator_attr(char const* expected, G const& g, T1 const& attr1, 
    T2 const& attr2)
{
    std::string s;
    std::back_insert_iterator<std::string> out(s);
    if (boost::spirit::karma::generate(out, g, attr1, attr2) && s == expected)
        std::cout << "ok" << std::endl;
    else
        std::cout << "fail" << std::endl;
}
//]

//[reference_karma_test_attr_delim
template <typename G, typename Delimiter, typename T>
void test_generator_attr_delim(char const* expected, G const& g, Delimiter const& d, T const& attr)
{
    std::string s;
    std::back_insert_iterator<std::string> out(s);
    if (boost::spirit::karma::generate_delimited(out, g, d, attr) && s == expected)
        std::cout << "ok" << std::endl;
    else
        std::cout << "fail" << std::endl;
}
//]

//[reference_karma_binary_test
template <typename G>
void test_binary_generator(char const* expected, std::size_t size, G const& g)
{
    std::string s;
    std::back_insert_iterator<std::string> out(s);
    if (boost::spirit::karma::generate(out, g) && !std::memcmp(s.c_str(), expected, size))
        std::cout << "ok" << std::endl;
    else
        std::cout << "fail" << std::endl;
}
//]

//[reference_karma_binary_test_attr
template <typename G, typename T>
void test_binary_generator_attr(char const* expected, std::size_t size, G const& g, T const& attr)
{
    std::string s;
    std::back_insert_iterator<std::string> out(s);
    if (boost::spirit::karma::generate(out, g, attr) && !std::memcmp(s.c_str(), expected, size))
        std::cout << "ok" << std::endl;
    else
        std::cout << "fail" << std::endl;
}
//]

//[reference_karma_stream_complex
// a simple complex number representation z = a + bi
struct complex
{
    complex (double a, double b)
      : a(a), b(b)
    {}

    double a;
    double b;
};

// define streaming operator for the type complex
std::ostream& 
operator<< (std::ostream& os, complex const& z)
{
    os << "{" << z.a << "," << z.b << "}";
    return os;
}
//]

//[reference_karma_auxiliary_attr_cast_data1
// this is just a test structure we want to use in place of an int
struct int_data
{
    int i;
};

// we provide a custom attribute transformation to allow its use as an int
namespace boost { namespace spirit { namespace traits
{
    template <>
    struct transform_attribute<int_data const, int>
    {
        static int pre(int_data const& d) { return d.i; }
    };
}}}
//]

int main()
{
    ///////////////////////////////////////////////////////////////////////////
    // Operators
    ///////////////////////////////////////////////////////////////////////////
    {
        //[reference_karma_using_declarations_sequence
        using boost::spirit::karma::double_;
        //]

        //[reference_karma_sequence
        test_generator_attr("1.0,2.0", double_ << ',' << double_, std::make_pair(1.0, 2.0));
        //]
    }

    {
        //[reference_karma_using_declarations_alternative
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
        using boost::spirit::karma::double_;
        using boost::spirit::karma::space;
        //]

        //[reference_karma_kleene
        std::vector<double> v;
        v.push_back(1.0);
        v.push_back(2.0);
        v.push_back(3.0);
        test_generator_attr_delim("1.0 2.0 3.0 ", *double_, space, v);
        //]
    }

    {
        //[reference_karma_using_declarations_plus
        using boost::spirit::karma::double_;
        using boost::spirit::karma::space;
        //]

        //[reference_karma_plus1
        std::vector<double> v1;
        v1.push_back(1.0);
        v1.push_back(2.0);
        v1.push_back(3.0);
        test_generator_attr_delim("1.0 2.0 3.0 ", +double_, space, v1);
        //]

        //[reference_karma_plus2
        std::vector<double> v2;               // empty container
        test_generator_attr("empty", +double_ | "empty", v2);
        //]
    }

    {
        //[reference_karma_using_declarations_list
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
        using boost::spirit::karma::double_;
        //]

        //[reference_karma_optional1
        boost::optional<double> val(1.0);
        test_generator_attr("1.0", -double_, val);
        test_generator_attr("2.0", -double_, 2.0);
        //]
    }
    {
        using boost::spirit::karma::double_;

        //[reference_karma_optional2
        boost::optional<double> val;          // empty optional
        test_generator_attr("", -double_, val);
        //]
    }

    {
        //[reference_karma_using_declarations_and_predicate
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
        //[reference_karma_using_declarations_alignment
        using boost::spirit::karma::double_;
        using boost::spirit::karma::left_align;
        using boost::spirit::karma::center;
        using boost::spirit::karma::right_align;
        //]

        //[reference_karma_alignment
        std::pair<double, double> p (1.0, 2.0);
        test_generator_attr("1.0     |2.0", left_align(8)[double_] << '|' << double_, p);
        test_generator_attr("   1.0  |2.0", center(8)[double_] << '|' << double_, p);
        test_generator_attr("     1.0|2.0", right_align(8)[double_] << '|' << double_, p);
        //]
    }

    {
        //[reference_karma_using_declarations_repeat
        using boost::spirit::karma::double_;
        using boost::spirit::karma::repeat;
        //]

        //[reference_karma_repeat
        std::vector<double> v;
        v.push_back(1.0);
        v.push_back(2.0);
        v.push_back(3.0);

        test_generator_attr("[1.0][2.0][3.0]", repeat['[' << double_ << ']'], v);
        test_generator_attr("[1.0][2.0]", repeat(2)['[' << double_ << ']'], v);

        // fails because of insufficient number of items
        test_generator_attr("", repeat(4)['[' << double_ << ']'], v);
        //]
    }

    {
        //[reference_karma_using_declarations_delimit
        using boost::spirit::karma::double_;
        using boost::spirit::karma::delimit;
        using boost::spirit::karma::verbatim;
        //]

        //[reference_karma_delimit
        test_generator_attr("[ 2.0 , 4.3 ] ", 
            delimit['[' << double_ << ',' << double_ << ']'], 2.0, 4.3);
        test_generator_attr("[*2.0*,*4.3*]*", 
            delimit('*')['[' << double_ << ',' << double_ << ']'], 2.0, 4.3);
        test_generator_attr("[2.0, 4.3 ] ", 
            delimit[verbatim['[' << double_ << ','] << double_ << ']'], 2.0, 4.3);
        //]
    }

    {
        //[reference_karma_using_declarations_upperlower
        using boost::spirit::karma::double_;
        using boost::spirit::ascii::upper;
        using boost::spirit::ascii::lower;
        //]

        //[reference_karma_upperlower
        test_generator_attr("abc:2.0e-06", lower["ABC:" << double_], 2e-6);
        test_generator_attr("ABC:2.0E-06", upper["abc:" << double_], 2e-6);
        //]
    }

    {
        //[reference_karma_using_declarations_maxwidth
        using boost::spirit::karma::double_;
        using boost::spirit::karma::maxwidth;
        using boost::spirit::karma::left_align;
        using boost::spirit::karma::right_align;
        //]

        //[reference_karma_maxwidth
        test_generator("01234", maxwidth(5)["0123456789"]);
        test_generator("  012", maxwidth(5)[right_align(12)["0123456789"]]);
        test_generator("0123    ", maxwidth(8)[left_align(8)["0123"]]);
        //]
    }

    {
        //[reference_karma_using_declarations_buffer
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
        using boost::spirit::karma::double_;
        using boost::spirit::karma::omit;
        //]

        //[reference_karma_omit
        std::pair<double, double> p (1.0, 2.0);
        test_generator_attr("2.0", omit[double_] << double_, p);
        //]
    }

    {
        //[reference_karma_using_declarations_bool
        using boost::spirit::karma::bool_;
        using boost::spirit::karma::lit;
        //]

        //[reference_karma_bool
        test_generator("true", lit(true));
        test_generator("false", bool_(false));
        test_generator_attr("true", bool_(true), true);
        test_generator_attr("", bool_(true), false);    // fails (as true != false)!
        test_generator_attr("false", bool_, false);
        //]
    }

    {
        //[reference_karma_using_declarations_int
        using boost::spirit::karma::int_;
        using boost::spirit::karma::lit;
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
        using boost::spirit::karma::uint_;
        using boost::spirit::karma::lit;
        //]

        //[reference_karma_uint
        test_generator("2", lit(2U));
        test_generator("2", uint_(2));
        test_generator_attr("2", uint_(2), 2);
        test_generator_attr("", uint_(2), 3);    // fails (as 2 != 3)!
        test_generator_attr("2", uint_, 2);
        //]
    }

    {
        //[reference_karma_using_declarations_real
        using boost::spirit::karma::double_;
        using boost::spirit::karma::lit;
        //]

        //[reference_karma_real
        test_generator("2.0", lit(2.0));
        test_generator("2.0", double_(2));
        test_generator_attr("2.0", double_(2.0), 2.0);
        test_generator_attr("", double_(2.0), 3.0);    // fails (as 2.0 != 3.0)!
        test_generator_attr("-2.0", double_, -2.0);

        test_generator_attr("1.234e05", double_, 1234.0e2);
        test_generator_attr("1.234e-06", double_, 0.000001234);
        //]
    }

    {
        //[reference_karma_using_declarations_char
        using boost::spirit::karma::lit;
        using boost::spirit::ascii::char_;
        //]

        //[reference_karma_char
        test_generator("A", 'A');
        test_generator("A", lit('A'));

        test_generator_attr("a", char_, 'a');
        test_generator("A", char_('A'));
        test_generator_attr("A", char_('A'), 'A');
        test_generator_attr("", char_('A'), 'B');         // fails (as 'A' != 'B')

        test_generator_attr("A", char_('A', 'Z'), 'A');
        test_generator_attr("", char_('A', 'Z'), 'a');    // fails (as 'a' does not belong to 'A'...'Z')

        test_generator_attr("k", char_("a-z0-9"), 'k');
        test_generator_attr("", char_("a-z0-9"), 'A');    // fails (as 'A' does not belong to "a-z0-9")
        //]
    }

    {
        //[reference_karma_using_declarations_char_class
        using boost::spirit::karma::alpha;
        using boost::spirit::karma::upper;
        //]

        //[reference_karma_char_class
        test_generator_attr("a", alpha, 'a');
        test_generator_attr("A", alpha, 'A');
        test_generator_attr("", alpha, '1');          // fails (as isalpha('1') is false)
        test_generator_attr("A", upper[alpha], 'a');
        test_generator_attr("", upper[alpha], 'a');   // fails (as isupper('a') is false)
        //]
    }

    ///////////////////////////////////////////////////////////////////////////
    // string
    {
        //[reference_karma_using_declarations_string
        using boost::spirit::karma::lit;
        using boost::spirit::ascii::string;
        //]

        //[reference_karma_string
        test_generator("abc", "abc");
        test_generator("abc", lit("abc"));
        test_generator("abc", lit(std::string("abc")));

        test_generator_attr("abc", string, "abc");
        test_generator("abc", string("abc"));
        test_generator("abc", string(std::string("abc")));

        test_generator_attr("abc", string("abc"), "abc");
        test_generator_attr("", string("abc"), "cba");     // fails (as "abc" != "cba")
        //]
    }

    ///////////////////////////////////////////////////////////////////////////
    // auxiliary
    {
        //[reference_karma_using_declarations_eol
        using boost::spirit::karma::eol;
        //]

        //[reference_karma_eol
        test_generator("\n", eol);
        test_generator("abc\n", "abc" << eol);
        //]
    }

    {
        //[reference_karma_using_declarations_attr_cast
        using boost::spirit::karma::attr_cast;
        //]

        //[reference_karma_attr_cast1
        int_data d = { 1 };
        test_generator("1", karma::attr_cast(karma::int_), d);
        //]
    }

    {
        //[reference_karma_using_declarations_eps
        using boost::spirit::karma::eps;
        using boost::phoenix::val;
        //]

        //[reference_karma_eps
        test_generator("abc", eps[std::cout << val("starting eps example")] << "abc");
        test_generator("abc", eps(true) << "abc");
        test_generator("", eps(false) << "abc");      // fails as eps expression is 'false'
        //]
    }

    {
        //[reference_karma_using_declarations_lazy
        namespace karma = boost::spirit::karma;
        using boost::spirit::karma::_1;
        using boost::spirit::ascii::string;
        using boost::phoenix::val;
        //]

        //[reference_karma_lazy
        test_generator_attr("abc", karma::lazy(val(string)), "abc");
        test_generator("abc", karma::lazy(val(string))[_1 = "abc"]);
        //]
    }

    ///////////////////////////////////////////////////////////////////////////
    // stream module
    {
        //[reference_karma_using_declarations_stream
        using boost::spirit::karma::stream;
        //]

        //[reference_karma_stream
        test_generator_attr("abc", stream, "abc");
        test_generator("abc", stream("abc"));
        test_generator_attr("{1.2,2.4}", stream, complex(1.2, 2.4));
        test_generator("{1.2,2.4}", stream(complex(1.2, 2.4)));
        //]
    }

    ///////////////////////////////////////////////////////////////////////////
    // binary module
    {
        //[reference_karma_using_declarations_native_binary
        using boost::spirit::karma::byte_;
        using boost::spirit::karma::word;
        using boost::spirit::karma::dword;
        using boost::spirit::karma::qword;
        //]

        //[reference_karma_native_binary_little
        test_binary_generator("\x01", 1, byte_(0x01));
        test_binary_generator("\x01\x02", 2, word(0x0201));
        test_binary_generator("\x01\x02\x03\x04", 4, dword(0x04030201));
        test_binary_generator("\x01\x02\x03\x04\x05\x06\x07\x08", 8, qword(0x0807060504030201LL));

        test_binary_generator_attr("\x01", 1, byte_, 0x01);
        test_binary_generator_attr("\x01\x02", 2, word, 0x0201);
        test_binary_generator_attr("\x01\x02\x03\x04", 4, dword, 0x04030201);
        test_binary_generator_attr("\x01\x02\x03\x04\x05\x06\x07\x08", 8, qword, 0x0807060504030201LL);
        //]

        //[reference_karma_native_binary_big
        test_binary_generator("\x01", 1, byte_(0x01));
        test_binary_generator("\x02\x01", 2, word(0x0201));
        test_binary_generator("\x04\x03\x02\x01", 4, dword(0x04030201));
        test_binary_generator("\x08\x07\x06\x05\x04\x03\x02\x01", 8, qword(0x0807060504030201LL));

        test_binary_generator_attr("\x01", 1, byte_, 0x01);
        test_binary_generator_attr("\x02\x01", 2, word, 0x0201);
        test_binary_generator_attr("\x04\x03\x02\x01", 4, dword, 0x04030201);
        test_binary_generator_attr("\x08\x07\x06\x05\x04\x03\x02\x01", 8, qword, 0x0807060504030201LL);
        //]
    }

    {
        //[reference_karma_using_declarations_little_binary
        using boost::spirit::karma::little_word;
        using boost::spirit::karma::little_dword;
        using boost::spirit::karma::little_qword;
        //]

        //[reference_karma_little_binary
        test_binary_generator("\x01\x02", 2, little_word(0x0201));
        test_binary_generator("\x01\x02\x03\x04", 4, little_dword(0x04030201));
        test_binary_generator("\x01\x02\x03\x04\x05\x06\x07\x08", 8, little_qword(0x0807060504030201LL));

        test_binary_generator_attr("\x01\x02", 2, little_word, 0x0201);
        test_binary_generator_attr("\x01\x02\x03\x04", 4, little_dword, 0x04030201);
        test_binary_generator_attr("\x01\x02\x03\x04\x05\x06\x07\x08", 8, little_qword, 0x0807060504030201LL);
        //]
    }

    {
        //[reference_karma_using_declarations_big_binary
        using boost::spirit::karma::big_word;
        using boost::spirit::karma::big_dword;
        using boost::spirit::karma::big_qword;
        //]

        //[reference_karma_big_binary
        test_binary_generator("\x02\x01", 2, big_word(0x0201));
        test_binary_generator("\x04\x03\x02\x01", 4, big_dword(0x04030201));
        test_binary_generator("\x08\x07\x06\x05\x04\x03\x02\x01", 8, big_qword(0x0807060504030201LL));

        test_binary_generator_attr("\x02\x01", 2, big_word, 0x0201);
        test_binary_generator_attr("\x04\x03\x02\x01", 4, big_dword, 0x04030201);
        test_binary_generator_attr("\x08\x07\x06\x05\x04\x03\x02\x01", 8, big_qword, 0x0807060504030201LL);
        //]
    }

    return 0;
}
