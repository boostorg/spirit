/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include "expression.hpp"

namespace client
{
    template <typename Iterator>
    expression<Iterator>::expression()
      : expression::base_type(expr)
    {
        qi::char_type char_;
        qi::uint_type uint_;
        qi::_2_type _2;
        qi::_3_type _3;
        qi::_4_type _4;

        using qi::on_error;
        using qi::fail;

        expr =
            additive_expr.alias()
            ;

        additive_expr =
            multiplicative_expr
            >> *(   (char_('+') > multiplicative_expr)
                |   (char_('-') > multiplicative_expr)
                )
            ;

        multiplicative_expr =
            unary_expr
            >> *(   (char_('*') > unary_expr)
                |   (char_('/') > unary_expr)
                )
            ;

        unary_expr =
                primary_expr
            |   (char_('-') > primary_expr)
            |   (char_('+') > primary_expr)
            ;

        primary_expr =
            uint_
            |   '(' > expr > ')'
            ;

        // Debugging and error handling and reporting support.
        BOOST_SPIRIT_DEBUG_NODE(additive_expr);
        BOOST_SPIRIT_DEBUG_NODE(multiplicative_expr);
        BOOST_SPIRIT_DEBUG_NODE(unary_expr);
        BOOST_SPIRIT_DEBUG_NODE(primary_expr);

        // Error handling
        on_error<fail>(expr, error_handler(_4, _3, _2));
    }
}


