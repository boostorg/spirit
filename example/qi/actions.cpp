#include <iostream>
#include <boost/spirit/include/qi.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/bind.hpp>

// Presented are various ways to attach semantic actions
//  * Using plain function pointers
//  * Using plain simple function objects
//  * Using plain boost.bind
//  * Using plain boost.lambda

using namespace boost::spirit;

void write(int& i)
{
    std::cout << i << std::endl;
}

struct write_action
{
    void operator()(int& i, unused_type, unused_type) const
    {
        std::cout << i << std::endl;
    }
};

int main()
{
    char const *s1 = "{42}", *e1 = s1 + std::strlen(s1);

    { // example using plain functions

        qi::parse(s1, e1, '{' >> int_[&write] >> '}');
    }

    { // example using simple function objects

        qi::parse(s1, e1, '{' >> int_[write_action()] >> '}');
    }

    { // example using boost.bind

        qi::parse(s1, e1, '{' >> int_[boost::bind(&write, _1)] >> '}');
    }

    { // example using boost.lambda

        using boost::lambda::_1;
        qi::parse(s1, e1, '{' >> int_[std::cout << _1 << '\n'] >> '}');
    }}

