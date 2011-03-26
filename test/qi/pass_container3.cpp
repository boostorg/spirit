//  Copyright (c) 2001-2011 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// compilation test only

#include <string>
#include <vector>

#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

#include <boost/variant.hpp>

//////////////////////////////////////////////////////////////////////////////
struct ast; // Forward declaration

typedef boost::variant<
    double, char, int, std::string, boost::recursive_wrapper<ast>
> ast_element;

struct ast 
{
    int op; 
    std::vector<ast_element> children;
    ast() {} 
};

BOOST_FUSION_ADAPT_STRUCT(
    ast,
    (int, op)
    (std::vector<ast_element>, children)
)

///////////////////////////////////////////////////////////////////////////////
int main()
{
    namespace qi = boost::spirit::qi;

    qi::rule<char const*, ast()> num_expr;
    num_expr = (*(qi::char_ >> num_expr))[ qi::_1 ];

    return 0;
}

