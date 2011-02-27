/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_CALC7_STATEMENT_HPP)
#define BOOST_SPIRIT_CALC7_STATEMENT_HPP

#include "expression.hpp"

namespace client
{
    ///////////////////////////////////////////////////////////////////////////////
    //  The statement grammar
    ///////////////////////////////////////////////////////////////////////////////
    template <typename Iterator>
    struct statement : qi::grammar<Iterator, ast::statement(), ascii::space_type>
    {
        statement();

        expression<Iterator> expr;
        qi::rule<Iterator, ast::statement(), ascii::space_type> start;
        qi::rule<Iterator, ast::variable_declaration(), ascii::space_type> variable_declaration;
        qi::rule<Iterator, ast::assignment(), ascii::space_type> assignment;
        qi::rule<Iterator, std::string(), ascii::space_type> identifier;


        //~ variable_declaration;
        //~ qi::rule<Iterator, std::string(), ascii::space_type> identifier;
        //~ qi::rule<Iterator, int(), ascii::space_type> var_ref;
        //~ qi::rule<Iterator, qi::locals<int>, ascii::space_type> assignment;
        //~ qi::rule<Iterator, void(int), ascii::space_type> assignment_rhs;
    };
}

#endif


