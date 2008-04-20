//  Copyright (c) 2001-2008 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(BOOST_SPIRIT_KARMA_TEST_FEB_23_2007_1221PM)
#define BOOST_SPIRIT_KARMA_TEST_FEB_23_2007_1221PM

#include <cstring>
#include <string>
#include <iterator>
#include <iostream>

#include <boost/function_output_iterator.hpp>
#include <boost/spirit/include/karma_generate.hpp>
#include <boost/spirit/include/karma_what.hpp>

namespace spirit_test
{

    ///////////////////////////////////////////////////////////////////////////
    struct display_type
    {
        template<typename T>
        void operator()(T const &) const
        {
            std::cout << typeid(T).name() << std::endl;
        }

        template<typename T>
        static void print() 
        {
            std::cout << typeid(T).name() << std::endl;
        }
    };

    display_type const display = {};

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

    template <typename Char>
    struct output_iterator
    {
        typedef std::basic_string<Char> string_type;
        typedef string_appender<string_type> appender_type;
        typedef boost::function_output_iterator<appender_type> type;
    };
    
    ///////////////////////////////////////////////////////////////////////////
    template <typename Char, typename Generator>
    inline bool test(Char const *expected, Generator const& g)
    {
        // we don't care about the result of the "what" function.
        // we only care that all generators have it:
        boost::spirit::karma::what(g);

        using boost::make_function_output_iterator;
        namespace karma = boost::spirit::karma;
        
        typedef std::basic_string<Char> string_type;
        
        string_type generated;

        bool result = karma::generate(make_function_output_iterator(
            make_string_appender(generated)), g);
                
        return result && generated == expected;
    }

    template <typename Char, typename Generator>
    inline bool test(std::basic_string<Char> const& expected, Generator const& g)
    {
        // we don't care about the result of the "what" function.
        // we only care that all generators have it:
        boost::spirit::karma::what(g);

        using boost::make_function_output_iterator;
        namespace karma = boost::spirit::karma;
        
        typedef std::basic_string<Char> string_type;
        
        string_type generated;

        bool result = karma::generate(make_function_output_iterator(
            make_string_appender(generated)), g);
                
        return result && generated == expected;
    }

    ///////////////////////////////////////////////////////////////////////////
    template <typename Char, typename Generator, typename Parameter>
    inline bool test(Char const *expected, Generator const& g, 
        Parameter const &parameter)
    {
        // we don't care about the result of the "what" function.
        // we only care that all generators have it:
        boost::spirit::karma::what(g);

        using boost::make_function_output_iterator;
        namespace karma = boost::spirit::karma;
        
        typedef std::basic_string<Char> string_type;
        
        string_type generated;

        bool result = karma::generate(make_function_output_iterator(
            make_string_appender(generated)), g, parameter);
                
        return result && generated == expected;
    }

    template <typename Char, typename Generator, typename Parameter>
    inline bool test(std::basic_string<Char> const& expected, Generator const& g, 
        Parameter const &parameter)
    {
        // we don't care about the result of the "what" function.
        // we only care that all generators have it:
        boost::spirit::karma::what(g);

        using boost::make_function_output_iterator;
        namespace karma = boost::spirit::karma;
        
        typedef std::basic_string<Char> string_type;
        
        string_type generated;

        bool result = karma::generate(make_function_output_iterator(
            make_string_appender(generated)), g, parameter);
                
        return result && generated == expected;
    }

    ///////////////////////////////////////////////////////////////////////////
    template <typename Char, typename Generator, typename Delimiter>
    inline bool test_delimited(Char const *expected, Generator const& g, 
        Delimiter const& d)
    {
        // we don't care about the result of the "what" function.
        // we only care that all generators have it:
        boost::spirit::karma::what(g);

        using boost::make_function_output_iterator;
        namespace karma = boost::spirit::karma;
        
        typedef std::basic_string<Char> string_type;
        
        string_type generated;

        bool result = karma::generate_delimited(make_function_output_iterator(
            make_string_appender(generated)), g, d);
                
        return result && generated == expected;
    }

    template <typename Char, typename Generator, typename Delimiter>
    inline bool test_delimited(std::basic_string<Char> const& expected, 
        Generator const& g, Delimiter const& d)
    {
        // we don't care about the result of the "what" function.
        // we only care that all generators have it:
        boost::spirit::karma::what(g);

        using boost::make_function_output_iterator;
        namespace karma = boost::spirit::karma;
        
        typedef std::basic_string<Char> string_type;
        
        string_type generated;

        bool result = karma::generate_delimited(make_function_output_iterator(
            make_string_appender(generated)), g, d);
                
        return result && generated == expected;
    }

    ///////////////////////////////////////////////////////////////////////////
    template <typename Char, typename Generator, typename Parameter,
        typename Delimiter>
    inline bool test_delimited(Char const *expected, Generator const& g, 
        Parameter const &parameter, Delimiter const& d)
    {
        // we don't care about the result of the "what" function.
        // we only care that all generators have it:
        boost::spirit::karma::what(g);

        using boost::make_function_output_iterator;
        namespace karma = boost::spirit::karma;
        
        typedef std::basic_string<Char> string_type;
        
        string_type generated;

        bool result = karma::generate_delimited(make_function_output_iterator(
            make_string_appender(generated)), g, parameter, d);
                
        return result && generated == expected;
    }

    template <typename Char, typename Generator, typename Parameter,
        typename Delimiter>
    inline bool test_delimited(std::basic_string<Char> const& expected, 
        Generator const& g, Parameter const &parameter, Delimiter const& d)
    {
        // we don't care about the result of the "what" function.
        // we only care that all generators have it:
        boost::spirit::karma::what(g);

        using boost::make_function_output_iterator;
        namespace karma = boost::spirit::karma;
        
        typedef std::basic_string<Char> string_type;
        
        string_type generated;

        bool result = karma::generate_delimited(make_function_output_iterator(
            make_string_appender(generated)), g, parameter, d);
                
        return result && generated == expected;
    }

    ///////////////////////////////////////////////////////////////////////////
    template <typename Generator>
    inline bool 
    binary_test(char const *expected, std::size_t size, 
        Generator const& g)
    {
        // we don't care about the result of the "what" function.
        // we only care that all generators have it:
        boost::spirit::karma::what(g);

        using boost::make_function_output_iterator;
        namespace karma = boost::spirit::karma;
        
        typedef std::basic_string<unsigned char> string_type;
        
        string_type generated;

        bool result = karma::generate(make_function_output_iterator(
            make_string_appender(generated)), g);
                
        return result && !std::memcmp(generated.c_str(), expected, size);
    }

    ///////////////////////////////////////////////////////////////////////////
    template <typename Generator, typename Parameter>
    inline bool 
    binary_test(char const *expected, std::size_t size, 
        Generator const& g, Parameter const &parameter)
    {
        // we don't care about the result of the "what" function.
        // we only care that all generators have it:
        boost::spirit::karma::what(g);

        using boost::make_function_output_iterator;
        namespace karma = boost::spirit::karma;
        
        typedef std::basic_string<unsigned char> string_type;
        
        string_type generated;

        bool result = karma::generate(make_function_output_iterator(
            make_string_appender(generated)), g, parameter);
                
        return result && !std::memcmp(generated.c_str(), expected, size);
    }

}   // namespace spirit_test

#endif // !BOOST_SPIRIT_KARMA_TEST_FEB_23_2007_1221PM
