//  Copyright (c) 2001-2010 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/config/warning_disable.hpp>
#include <boost/detail/lightweight_test.hpp>

#include <boost/spirit/include/karma_auxiliary.hpp>
#include <boost/spirit/include/karma_char.hpp>
#include <boost/spirit/include/karma_string.hpp>
#include <boost/spirit/include/karma_operator.hpp>
#include <boost/spirit/include/karma_directive.hpp>
#include <boost/spirit/include/karma_generate.hpp>
#include <boost/spirit/include/karma_nonterminal.hpp>

#include "test.hpp"

namespace fusion = boost::fusion;

template <typename T>
inline std::vector<T> 
make_vector(T const& t1, T const& t2)
{
    std::vector<T> v;
    v.push_back(t1);
    v.push_back(t2);
    return v;
}

int main()
{
    using spirit_test::test;
    using boost::spirit::karma::symbols;

    { // basics
        symbols<char, std::string> sym;

        sym.add
            ('j', "Joel")
            ('h', "Hartmut")
            ('t', "Tom")
            ('k', "Kim")
        ;

        boost::mpl::true_ f = 
            boost::mpl::bool_<boost::spirit::traits::is_generator<
                symbols<char, std::string> >::value>();

        // silence stupid compiler warnings 
        // i.e. MSVC warning C4189: 'f' : local variable is initialized but not referenced
        BOOST_TEST((f.value));

        BOOST_TEST((test("Joel", sym, 'j')));
        BOOST_TEST((test("Hartmut", sym, 'h')));
        BOOST_TEST((test("Tom", sym, 't')));
        BOOST_TEST((test("Kim", sym, 'k')));
        BOOST_TEST((!test("", sym, 'x')));

        // test copy
        symbols<char, std::string> sym2;
        sym2 = sym;
        BOOST_TEST((test("Joel", sym2, 'j')));
        BOOST_TEST((test("Hartmut", sym2, 'h')));
        BOOST_TEST((test("Tom", sym2, 't')));
        BOOST_TEST((test("Kim", sym2, 'k')));
        BOOST_TEST((!test("", sym2, 'x')));

        // make sure it plays well with other generators
        BOOST_TEST((test("Joelyo", sym << "yo", 'j')));

        sym.remove
            ('j')
            ('h')
        ;

        BOOST_TEST((!test("", sym, 'j')));
        BOOST_TEST((!test("", sym, 'h')));
    }

    { // advanced
        using boost::spirit::karma::rule;
        using boost::spirit::karma::lit;

        typedef spirit_test::output_iterator<char>::type output_iterator_type;

        symbols<char, rule<output_iterator_type> > sym;

        rule<output_iterator_type> r1 = lit("Joel");
        rule<output_iterator_type> r2 = lit("Hartmut");
        rule<output_iterator_type> r3 = lit("Tom");
        rule<output_iterator_type> r4 = lit("Kim");
        
        sym.add
            ('j', r1.alias())
            ('h', r2.alias())
            ('t', r3.alias())
            ('k', r4.alias())
        ;

        boost::mpl::true_ f = 
            boost::mpl::bool_<boost::spirit::traits::is_generator<
                symbols<char, rule<output_iterator_type> > >::value>();

        // silence stupid compiler warnings 
        // i.e. MSVC warning C4189: 'f' : local variable is initialized but not referenced
        BOOST_TEST((f.value));

        BOOST_TEST((test("Joel", sym, 'j')));
        BOOST_TEST((test("Hartmut", sym, 'h')));
        BOOST_TEST((test("Tom", sym, 't')));
        BOOST_TEST((test("Kim", sym, 'k')));
        BOOST_TEST((!test("", sym, 'x')));

        // test copy
        symbols<char, rule<output_iterator_type> > sym2;
        sym2 = sym;
        BOOST_TEST((test("Joel", sym2, 'j')));
        BOOST_TEST((test("Hartmut", sym2, 'h')));
        BOOST_TEST((test("Tom", sym2, 't')));
        BOOST_TEST((test("Kim", sym2, 'k')));
        BOOST_TEST((!test("", sym2, 'x')));

        // make sure it plays well with other generators
        BOOST_TEST((test("Joelyo", sym << "yo", 'j')));

        sym.remove
            ('j')
            ('h')
        ;

        BOOST_TEST((!test("", sym, 'j')));
        BOOST_TEST((!test("", sym, 'h')));
    }

    { // more advanced
        using boost::spirit::karma::rule;
        using boost::spirit::karma::lit;
        using boost::spirit::karma::string;

        typedef spirit_test::output_iterator<char>::type output_iterator_type;

        symbols<char, rule<output_iterator_type, std::string()> > sym;
        rule<output_iterator_type, std::string()> r1 = string;
        
        sym.add
            ('j', r1.alias())
            ('h', r1.alias())
            ('t', r1.alias())
            ('k', r1.alias())
        ;

        boost::mpl::true_ f = 
            boost::mpl::bool_<boost::spirit::traits::is_generator<
                symbols<char, std::string> >::value>();

        // silence stupid compiler warnings 
        // i.e. MSVC warning C4189: 'f' : local variable is initialized but not referenced
        BOOST_TEST((f.value));

        BOOST_TEST((test("Joel", sym, fusion::make_vector('j', "Joel"))));
        BOOST_TEST((test("Hartmut", sym, fusion::make_vector('h', "Hartmut"))));
        BOOST_TEST((test("Tom", sym, fusion::make_vector('t', "Tom"))));
        BOOST_TEST((test("Kim", sym, fusion::make_vector('k', "Kim"))));
        BOOST_TEST((!test("", sym, 'x')));

        // test copy
        symbols<char, rule<output_iterator_type, std::string()> > sym2;
        sym2 = sym;
        BOOST_TEST((test("Joel", sym2, fusion::make_vector('j', "Joel"))));
        BOOST_TEST((test("Hartmut", sym2, fusion::make_vector('h', "Hartmut"))));
        BOOST_TEST((test("Tom", sym2, fusion::make_vector('t', "Tom"))));
        BOOST_TEST((test("Kim", sym2, fusion::make_vector('k', "Kim"))));
        BOOST_TEST((!test("", sym2, 'x')));

        // make sure it plays well with other generators
        BOOST_TEST((test("Joelyo", sym << "yo", fusion::make_vector('j', "Joel"))));

        sym.remove
            ('j')
            ('h')
        ;

        BOOST_TEST((!test("", sym, 'j')));
        BOOST_TEST((!test("", sym, 'h')));
    }

    { // more advanced
        using boost::spirit::karma::rule;
        using boost::spirit::karma::lit;
        using boost::spirit::karma::char_;

        typedef spirit_test::output_iterator<char>::type output_iterator_type;

        symbols<char, rule<output_iterator_type, char()> > sym;
        rule<output_iterator_type, char()> r1 = char_;
        
        sym.add
            ('j', r1.alias())
            ('h', r1.alias())
            ('t', r1.alias())
            ('k', r1.alias())
        ;

        boost::mpl::true_ f = 
            boost::mpl::bool_<boost::spirit::traits::is_generator<
                symbols<char, rule<output_iterator_type, char()> > >::value>();

        // silence stupid compiler warnings 
        // i.e. MSVC warning C4189: 'f' : local variable is initialized but not referenced
        BOOST_TEST((f.value));

        BOOST_TEST((test("J", sym, make_vector('j', 'J'))));
        BOOST_TEST((test("H", sym, make_vector('h', 'H'))));
        BOOST_TEST((test("T", sym, make_vector('t', 'T'))));
        BOOST_TEST((test("K", sym, make_vector('k', 'K'))));
        BOOST_TEST((!test("", sym, 'x')));

        // test copy
        symbols<char, rule<output_iterator_type, char()> > sym2;
        sym2 = sym;
        BOOST_TEST((test("J", sym2, make_vector('j', 'J'))));
        BOOST_TEST((test("H", sym2, make_vector('h', 'H'))));
        BOOST_TEST((test("T", sym2, make_vector('t', 'T'))));
        BOOST_TEST((test("K", sym2, make_vector('k', 'K'))));
        BOOST_TEST((!test("", sym2, 'x')));

        // make sure it plays well with other generators
        BOOST_TEST((test("Jyo", sym << "yo", make_vector('j', 'J'))));

        sym.remove
            ('j')
            ('h')
        ;

        BOOST_TEST((!test("", sym, 'j')));
        BOOST_TEST((!test("", sym, 'h')));
    }

    { // basics
        symbols<std::string> sym;

        sym.add
            ("Joel")
            ("Hartmut")
            ("Tom")
            ("Kim")
        ;

        boost::mpl::true_ f = 
            boost::mpl::bool_<boost::spirit::traits::is_generator<
                symbols<char, std::string> >::value>();

        // silence stupid compiler warnings 
        // i.e. MSVC warning C4189: 'f' : local variable is initialized but not referenced
        BOOST_TEST((f.value));

        BOOST_TEST((test("Joel", sym, "Joel")));
        BOOST_TEST((test("Hartmut", sym, "Hartmut")));
        BOOST_TEST((test("Tom", sym, "Tom")));
        BOOST_TEST((test("Kim", sym, "Kim")));
        BOOST_TEST((!test("", sym, "X")));

        // test copy
        symbols<std::string> sym2;
        sym2 = sym;
        BOOST_TEST((test("Joel", sym2, "Joel")));
        BOOST_TEST((test("Hartmut", sym2, "Hartmut")));
        BOOST_TEST((test("Tom", sym2, "Tom")));
        BOOST_TEST((test("Kim", sym2, "Kim")));
        BOOST_TEST((!test("", sym2, "X")));

        // make sure it plays well with other generators
        BOOST_TEST((test("Joelyo", sym << "yo", "Joel")));

        sym.remove
            ("Joel")
            ("Hartmut")
        ;

        BOOST_TEST((!test("", sym, "Joel")));
        BOOST_TEST((!test("", sym, "Hartmut")));
    }

    { // lower/upper handling
        using namespace boost::spirit::ascii;
        using boost::spirit::karma::lower;
        using boost::spirit::karma::upper;

        symbols<char, std::string> sym;
        sym.add
            ('j', "Joel")
            ('h', "Hartmut")
            ('t', "Tom")
            ('k', "Kim")
        ;

        BOOST_TEST((test("joel", lower[sym], 'j')));
        BOOST_TEST((test("hartmut", lower[sym], 'h')));
        BOOST_TEST((test("tom", lower[sym], 't')));
        BOOST_TEST((test("kim", lower[sym], 'k')));

        BOOST_TEST((test("JOEL", upper[sym], 'j')));
        BOOST_TEST((test("HARTMUT", upper[sym], 'h')));
        BOOST_TEST((test("TOM", upper[sym], 't')));
        BOOST_TEST((test("KIM", upper[sym], 'k')));

        // make sure it plays well with other generators
        BOOST_TEST((test("joelyo", lower[sym] << "yo", 'j')));
        BOOST_TEST((test("JOELyo", upper[sym] << "yo", 'j')));
    }

    return boost::report_errors();
}
