/*=============================================================================
    Copyright (c) 2001-2010 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include <boost/detail/lightweight_test.hpp>
#include <boost/config/warning_disable.hpp>

#include <input/parse_sexpr_impl.hpp>
#include <scheme/compiler.hpp>
#include <utree/io.hpp>
#include <iostream>
#include <fstream>

///////////////////////////////////////////////////////////////////////////////
//  Main program
///////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
    using scheme::utree;

    { // testing the c++ side

        using scheme::if_;
        using scheme::plus;
        using scheme::times;
        using scheme::minus;
        using scheme::lte;
        using scheme::_1;
        using scheme::_2;
        using scheme::lambda;

        BOOST_TEST(plus(11, 22, 33)         ()          == utree(66));
        BOOST_TEST(plus(11, 22, _1)         (33)        == utree(66));
        BOOST_TEST(plus(11, _1, _2)         (22, 33)    == utree(66));
        BOOST_TEST(plus(11, plus(_1, _2))   (22, 33)    == utree(66));

        lambda factorial;
        factorial = if_(lte(_1, 0), 1, times(_1, factorial(minus(_1, 1))));

        BOOST_TEST(factorial(_1)            (10)        == utree(3628800));
    }

    BOOST_TEST(argc > 1);
    char const* filename = filename = argv[1];
    std::ifstream in(filename, std::ios_base::in);

    BOOST_TEST(in);

    // Ignore the BOM marking the beginning of a UTF-8 file in Windows
    char c = in.peek();
    if (c == '\xef')
    {
        char s[3];
        in >> s[0] >> s[1] >> s[2];
        s[3] = '\0';
        BOOST_TEST(s != std::string("\xef\xbb\xbf"));
    }

    scheme::utree program;
    BOOST_TEST(scheme::input::parse_sexpr_list(in, program, filename));

    scheme::environment env;
    scheme::build_basic_environment(env);
    scheme::actor_list fragments;
    scheme::actor_list flist;
    compile_all(program, env, flist, fragments, filename);

    scheme::actor_list::iterator i = flist.begin();

    BOOST_TEST((*i++)(555) == 1110);
    BOOST_TEST((*i++)() == 123);
    BOOST_TEST((*i++)() == 246);
    BOOST_TEST((*i++)(5) == 120);
    BOOST_TEST((*i++)() == 3628800);
    BOOST_TEST((*i++)(5) == 5);
    BOOST_TEST((*i++)() == 55);
    BOOST_TEST((*i++)() == 21);

    return boost::report_errors();
}



