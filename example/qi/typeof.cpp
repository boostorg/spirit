/*=============================================================================
    Copyright (c) 2001-2009 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/typeof/typeof.hpp>
#include <iostream>
#include <string>

///////////////////////////////////////////////////////////////////////////////
//  Main program
///////////////////////////////////////////////////////////////////////////////

#define BOOST_SPIRIT_AUTO(domain_, name, expr)                                  \
    typedef BOOST_TYPEOF(expr) name##expr_type;                                 \
    BOOST_SPIRIT_ASSERT_MATCH(boost::spirit::domain_::domain, name##expr_type); \
    BOOST_AUTO(name, boost::proto::deep_copy(expr));                            \
    //

int
main()
{
    using boost::spirit::ascii::space;
    using boost::spirit::ascii::char_;
    using boost::spirit::qi::parse;
    using boost::spirit::qi::lit;
    typedef std::string::const_iterator iterator_type;
    
    BOOST_SPIRIT_AUTO(qi, comment_pre, lit("/*"));
    BOOST_SPIRIT_AUTO(qi, comment_mid, char_ - "*/");
    BOOST_SPIRIT_AUTO(qi, comment_post, lit("*/"));
    BOOST_SPIRIT_AUTO(qi, comment, comment_pre >> *comment_mid >> comment_post);

    std::string str = "/*This is a comment*/";
    std::string::const_iterator iter = str.begin();
    std::string::const_iterator end = str.end();
    bool r = parse(iter, end, comment);

    if (r && iter == end)
    {
        std::cout << "-------------------------\n";
        std::cout << "Parsing succeeded\n";
        std::cout << "-------------------------\n";
    }
    else
    {
        std::string rest(iter, end);
        std::cout << "-------------------------\n";
        std::cout << "Parsing failed\n";
        std::cout << "stopped at: \": " << rest << "\"\n";
        std::cout << "-------------------------\n";
    }

    return 0;
}


