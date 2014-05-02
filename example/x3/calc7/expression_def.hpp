/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_X3_CALC7_EXPRESSION_HPP)
#define BOOST_SPIRIT_X3_CALC7_EXPRESSION_HPP

#include <boost/spirit/home/x3.hpp>
#include "ast.hpp"

namespace client
{
    namespace x3 = boost::spirit::x3;

    ///////////////////////////////////////////////////////////////////////////////
    // rule IDs
    ///////////////////////////////////////////////////////////////////////////////
    typedef x3::identity<class expression> expression_id;
    typedef x3::identity<class term> term_id;
    typedef x3::identity<class factor> factor_id;

    ///////////////////////////////////////////////////////////////////////////////
    //  The calculator grammar
    ///////////////////////////////////////////////////////////////////////////////
    namespace calculator_grammar
    {
        using x3::uint_;
        using x3::char_;

        x3::rule<expression_id, ast::expression> const expression("expression");
        x3::rule<term_id, ast::expression> const term("term");
        x3::rule<factor_id, ast::operand> const factor("factor");

        auto const expression_def =
            term
            >> *(   (char_('+') > term)
                |   (char_('-') > term)
                )
            ;

        auto const term_def =
            factor
            >> *(   (char_('*') > factor)
                |   (char_('/') > factor)
                )
            ;

        auto const factor_def =
                uint_
            |   '(' > expression > ')'
            |   (char_('-') > factor)
            |   (char_('+') > factor)
            ;

        auto const calculator = x3::grammar(
                "calculator"
              , expression = expression_def
              , term = term_def
              , factor = factor_def
            );
    }

    using calculator_grammar::calculator;

    template <typename Iterator, typename Skipper>
    inline auto expression_parser(Skipper const& skipper)
    -> x3::any_parser<Iterator, ast::expression, decltype(x3::make_context<x3::skipper_tag>(skipper))>
    {
        return { calculator };
    }

}

#endif
