//  Copyright (c) 2001-2010 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/config/warning_disable.hpp>

#define BOOST_SPIRIT_UNICODE

#include <iostream>
#include <fstream>
#include <iterator>

#include <utree/utree.hpp>
#include <utree/operators.hpp>
#include <qi/parse_qiexpr.hpp>
#include <qi/generate_qiexpr.hpp>

///////////////////////////////////////////////////////////////////////////////
//  Main program
///////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
    std::string str;
    while (std::getline(std::cin, str))
    {
        if (str.empty() || str[0] == 'q' || str[0] == 'Q')
            break;
        str += '\n';

        scheme::utree result;
        if (scheme::input::parse_qi_expr(str, result))
        {
            std::string str;
            if (scheme::output::generate_qiexpr(result, str))
            {
                std::cout << str << std::endl;
            }
            else
            {
                std::cout << "generate error: " << result << std::endl;
            }
        }
        else
        {
            std::cout << "parse error" << std::endl;
        }
    }
    return 0;
}
