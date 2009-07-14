//   Copyright (c) 2002-2009 Joel Hartmut Kaiser
//   Copyright (c) 2002-2009 Joel de Guzman
// 
//   Distributed under the Boost Software License, Version 1.0. (See accompanying
//   file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/karma.hpp>
#include <boost/format.hpp>

#include <iostream>

#include "high_resolution_timer.hpp"

#define NUMITERATIONS 1000000

///////////////////////////////////////////////////////////////////////////////
//  policy for real_generator, which forces to output trailing zeros in the 
//  fractional part
template <typename T>
struct double3_policy : boost::spirit::karma::real_policies<T>   
{
    //  we want to generate up to 3 fractional digits
    static unsigned int precision(T) { return 3; }
};

typedef boost::spirit::karma::real_generator<double, double3_policy<double> > 
    double3_type;
double3_type const double3 = double3_type();

void format_performance_karma()
{
    using boost::spirit::karma::generate;

    char buffer[256];

    util::high_resolution_timer t;

    for (int i = 0; i < NUMITERATIONS; ++i) {
        char *p = buffer;

        generate(p, double3, 12345.12345);

        *p = '\0';
    }

    std::cout << "karma:  " << t.elapsed() << std::endl;
//     std::cout << buffer << std::endl;
}

void format_performance_karma_rule()
{
    using boost::spirit::karma::generate;

    boost::spirit::karma::rule<char*, double()> r;

    char buffer[256];
    r %= double3;

    util::high_resolution_timer t;

    for (int i = 0; i < NUMITERATIONS; ++i) {
        char *p = buffer;

        generate(p, r, 12345.12345);

        *p = '\0';
    }

    std::cout << "karma (rule):  " << t.elapsed() << std::endl;
//     std::cout << buffer << std::endl;
}

void format_performance_karma_direct()
{
    using boost::spirit::karma::generate;
    using boost::spirit::karma::real_inserter;

    typedef real_inserter<double, double3_policy<double> > inserter;

    char buffer[256];

    util::high_resolution_timer t;

    for (int i = 0; i < NUMITERATIONS; ++i) {
        char *p = buffer;

        inserter::call(p, 12345.12345, double3_policy<double>());

        *p = '\0';
    }

    std::cout << "karma (direct):  " << t.elapsed() << std::endl;
//     std::cout << buffer << std::endl;
}

void format_performance_karma_string()
{
    using boost::spirit::karma::generate;

    std::string generated;
    std::back_insert_iterator<std::string> sink(generated);

    util::high_resolution_timer t;

    for (int i = 0; i < NUMITERATIONS; ++i) {
        generated.clear();

        generate(sink, double3, 12345.12345);
    }

    std::cout << "karma (string): " << t.elapsed() << std::endl;
//     std::cout << generated << std::endl;
}

// Boost.Format  
void format_performance_boost_format()
{
    std::stringstream strm;

    util::high_resolution_timer t;

    for (int i = 0; i < NUMITERATIONS; ++i) {
        strm.str("");
        strm << boost::format("%f") % 12345.12345;
    }

    std::cout << "format: " << t.elapsed() << std::endl;
//     std::cout << strm.str() << std::endl;
}

void format_performance_printf()
{
    util::high_resolution_timer t;

    char buffer[256];
    for (int i = 0; i < NUMITERATIONS; ++i) {
        sprintf(buffer, "%f", 12345.12345);
    }

    std::cout << "printf: " << t.elapsed() << std::endl;
//     std::cout << buffer << std::endl;
}

void format_performance_iostreams()
{
    std::stringstream strm;

    util::high_resolution_timer t;

    for (int i = 0; i < NUMITERATIONS; ++i) {
        strm.str("");

        strm << 12345.12345;
    }

    std::cout << "iostreams: " << t.elapsed() << std::endl;
//     std::cout << strm.str() << std::endl;
}

///////////////////////////////////////////////////////////////////////////////
int main()
{
    format_performance_printf();
    format_performance_iostreams();
    format_performance_boost_format();
    format_performance_karma();
    format_performance_karma_string();
    format_performance_karma_rule();
    format_performance_karma_direct();
    return 0;
}

