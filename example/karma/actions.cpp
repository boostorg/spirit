/*=============================================================================
    Copyright (c) 2001-2008 Hartmut Kaiser
    Copyright (c) 2001-2008 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include <iostream>
#include <strstream>

#include <boost/spirit/include/karma.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/bind.hpp>
#include <boost/function_output_iterator.hpp>

// Presented are various ways to attach semantic actions
//  * Using plain function pointers
//  * Using plain simple function objects
//  * Using plain boost.bind
//  * Using plain boost.lambda

using namespace boost::spirit;

void read(int& i)
{
    i = 42;
}

struct read_action
{
    void operator()(int& i, unused_type, unused_type) const
    {
        i = 42;
    }
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
int main()
{
    using boost::make_function_output_iterator;

    { // example using plain functions
        std::string generated;
        bool result = karma::generate(
            make_function_output_iterator(make_string_appender(generated)), 
            '{' << int_[&read] << '}');

        if (result)
            std::cout << "Simple function: " << generated << std::endl;
    }

    { // example using simple function objects
        std::string generated;
        bool result = karma::generate(
            make_function_output_iterator(make_string_appender(generated)), 
            '{' << int_[read_action()] << '}');

        if (result)
            std::cout << "Simple function object: " << generated << std::endl;
    }

    { // example using boost.bind
        std::string generated;
        bool result = karma::generate(
            make_function_output_iterator(make_string_appender(generated)), 
            '{' << int_[boost::bind(&read, _1)] << '}');

        if (result)
            std::cout << "Boost.Bind: " << generated << std::endl;
    }

    { // example using boost.lambda
        using boost::lambda::_1;

        std::string generated;
        std::stringstream strm("42");
        
        bool result = karma::generate(
            make_function_output_iterator(make_string_appender(generated)), 
            '{' << int_[strm >> _1] << '}');

        if (result)
            std::cout << "Boost.Lambda: " << generated << std::endl;
    }
    
    return 0;
}

