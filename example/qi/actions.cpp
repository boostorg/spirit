/*=============================================================================
    Copyright (c) 2001-2008 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/bind.hpp>

#include <iostream>

// Presented are various ways to attach semantic actions
//  * Using plain function pointer
//  * Using simple function object
//  * Using boost.bind
//  * Using boost.lambda

using namespace boost::spirit;

void write(int const& i)
{
    std::cout << i << std::endl;
}

struct write_action
{
    void operator()(int const& i, unused_type, unused_type) const
    {
        std::cout << i << std::endl;
    }
};

int main()
{
    { // example using plain function

        char const *s1 = "{42}", *e1 = s1 + std::strlen(s1);
        qi::parse(s1, e1, '{' >> int_[&write] >> '}');
    }

    { // example using simple function object

        char const *s1 = "{43}", *e1 = s1 + std::strlen(s1);
        qi::parse(s1, e1, '{' >> int_[write_action()] >> '}');
    }

    { // example using boost.bind

        char const *s1 = "{44}", *e1 = s1 + std::strlen(s1);
        qi::parse(s1, e1, '{' >> int_[boost::bind(&write, _1)] >> '}');
    }

    { // example using boost.lambda

        namespace lambda = boost::lambda;
        char const *s1 = "{45}", *e1 = s1 + std::strlen(s1);
        qi::parse(s1, e1, '{' >> int_[std::cout << lambda::_1 << '\n'] >> '}');
    }

    return 0;
}




