//  Copyright (c) 2001-2010 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/config/warning_disable.hpp>

#define BOOST_SPIRIT_UNICODE

#include <iostream>
#include <fstream>
#include <iterator>

#include <boost/detail/lightweight_test.hpp>
#include <boost/lexical_cast.hpp>

#include <utree/utree.hpp>
#include <qi/parse_qiexpr.hpp>
#include <output/generate_sexpr.hpp>

///////////////////////////////////////////////////////////////////////////////
bool parse_rhs(std::string const& str, scheme::utree& result) 
{
    if (scheme::input::parse_qi_expr(str, result))
    {
        if (scheme::output::generate_sexpr(std::cout, result))
        {
            std::cout << std::endl;
            return true;
        }
        else
        {
            std::cout << "generate error" << std::endl;
        }
    }
    else
    {
        std::cout << "parse error" << std::endl;
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////////
bool parse_rule(std::string str) 
{
    // construct a rule
    str = "name = " + str;

    // parse it
    scheme::utree result;
    if (scheme::input::parse_qi_rule(str, result))
    {
        if (scheme::output::generate_sexpr(std::cout, result))
        {
            std::cout << std::endl;
            return true;
        }
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////////
bool parse_grammar(std::string str) 
{
    // parse it
    scheme::utree result;
    if (scheme::input::parse_qi_grammar(str, result))
    {
        if (scheme::output::generate_sexpr_list(std::cout, result))
        {
            std::cout << std::endl;
            return true;
        }
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////////
//  Main program
///////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
    std::string rules;
    int i = 0;

    std::string str;
    while (std::getline(std::cin, str))
    {
        if (str.empty() || str[0] == 'q' || str[0] == 'Q')
            break;
        str += '\n';

        bool r = false;
        scheme::utree result;
        BOOST_TEST(r = parse_rhs(str, result));
        if (r && result.which() != scheme::utree_type::nil_type)
        {
            BOOST_TEST(r = parse_rule(str));
            if (r) 
            {
                rules += "rule" + boost::lexical_cast<std::string>(++i) 
                            + " = " + str + "\n";
            }
        }
    }

    // now test grammar rule
    BOOST_TEST(parse_grammar(rules));

    return boost::report_errors();
}
