/*=============================================================================
    Copyright (c) 2001-2015 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_X3_REPR_REXPR_DEF_HPP)
#define BOOST_SPIRIT_X3_REPR_REXPR_DEF_HPP

#include "annotation.hpp"
#include "ast.hpp"
#include "ast_adapted.hpp"
#include "error_handler.hpp"
#include "rexpr.hpp"

#include <boost/spirit/home/x3.hpp>

namespace rexpr { namespace parser
{
    namespace x3 = boost::spirit::x3;
    namespace ascii = boost::spirit::x3::ascii;

    using x3::lit;
    using x3::lexeme;

    using ascii::char_;
    using ascii::string;

    ///////////////////////////////////////////////////////////////////////////
    // Rule IDs
    ///////////////////////////////////////////////////////////////////////////

    struct rexpr_value_class;
    struct rexpr_key_value_class;
    struct rexpr_inner_class;
    struct rexpr_class;

    ///////////////////////////////////////////////////////////////////////////
    // Rules
    ///////////////////////////////////////////////////////////////////////////

    x3::rule<rexpr_value_class, ast::rexpr_value>
        rexpr_value = "rexpr_value";

    x3::rule<rexpr_key_value_class, ast::rexpr_key_value>
        rexpr_key_value = "rexpr_key_value";

    x3::rule<rexpr_inner_class, ast::rexpr>
        rexpr_inner = "rexpr";

    x3::rule<rexpr_class, ast::rexpr>
        rexpr = "rexpr";

    ///////////////////////////////////////////////////////////////////////////
    // Grammar
    ///////////////////////////////////////////////////////////////////////////

    auto const quoted_string =
        lexeme['"' >> *(char_ - '"') >> '"'];

    auto const rexpr_value_def =
        quoted_string | rexpr_inner;

    auto const rexpr_key_value_def =
        quoted_string > '=' > rexpr_value;

    auto const rexpr_inner_def =
        '{' > *rexpr_key_value > '}';

    auto const rexpr_def = rexpr_inner_def;

    BOOST_SPIRIT_DEFINE(rexpr_value, rexpr, rexpr_inner, rexpr_key_value);

    ///////////////////////////////////////////////////////////////////////////
    // Annotation and Error handling
    ///////////////////////////////////////////////////////////////////////////

    // We want these to be annotated with the iterator position (see annotation.hpp)
    struct rexpr_value_class : annotation_base {};
    struct rexpr_key_value_class : annotation_base {};
    struct rexpr_inner_class : annotation_base {};

    // We want error-handling only for the start (outermost) rexpr
    // rexpr is the same as rexpr_inner but without error-handling (see error_handler.hpp)
    struct rexpr_class :
        annotation_base, error_handler_base {};
}}

namespace rexpr
{
    parser::rexpr_type const& rexpr()
    {
        return parser::rexpr;
    }
}

#endif
