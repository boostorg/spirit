/*=============================================================================
    Copyright (c) 2001-2008 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include <boost/detail/lightweight_test.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/bind.hpp>

using namespace boost::spirit;

int x = 0;

void fun1(int const& i)
{
    x += i;
}

void fun2(int i)
{
    x += i;
}

void fun3(int& i)
{
    x += i;
    i = 0;
}

struct fun_action
{
    void operator()(int const& i, unused_type, unused_type) const
    {
        x += i;
    }
};

int main()
{
    {
        char const *s1 = "{42}", *e1 = s1 + std::strlen(s1);
        qi::parse(s1, e1, '{' >> int_[&fun1] >> '}');
    }

    {
        char const *s1 = "{42}", *e1 = s1 + std::strlen(s1);
        qi::parse(s1, e1, '{' >> int_[&fun2] >> '}');
    }

    {
        char const *s1 = "{42}", *e1 = s1 + std::strlen(s1);
        qi::parse(s1, e1, '{' >> int_[&fun3] >> '}');
    }

    {
        char const *s1 = "{42}", *e1 = s1 + std::strlen(s1);
        qi::parse(s1, e1, '{' >> int_[fun_action()] >> '}');
    }

    {
        char const *s1 = "{42}", *e1 = s1 + std::strlen(s1);
        qi::parse(s1, e1, '{' >> int_[boost::bind(&fun, _1)] >> '}');
    }

    {
        using boost::lambda::_1;
        using boost::lambda::var;
        char const *s1 = "{42}", *e1 = s1 + std::strlen(s1);
        qi::parse(s1, e1, '{' >> int_[var(x) += _1] >> '}');
    }

    BOOST_TEST(x == (42*6));
    return 0;
}




