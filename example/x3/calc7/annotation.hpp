/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_X3_CALC7_ANNOTATION_HPP)
#define BOOST_SPIRIT_X3_CALC7_ANNOTATION_HPP

#include <map>
#include "ast.hpp"

namespace client
{
    // this id is used to tag the position_cache so we can
    // access it from the x3 context
    struct position_cache_tag;
}

namespace client { namespace ast
{
    ///////////////////////////////////////////////////////////////////////////////
    //  The on_success handler links the AST to a vector of iterator positions
    //  for the purpose of subsequent semantic error handling when the
    //  program is being compiled. See x3::position_cache in x3/support/ast
    ///////////////////////////////////////////////////////////////////////////////


    template <typename ID, typename Iterator, typename Context>
    inline void
    on_success(ID, Iterator const& first, Iterator const& last, operand& ast, Context const& context)
    {
        auto& pos_cache = x3::get<position_cache_tag>(context);
        auto annotate = [&](auto& node){ pos_cache.annotate(node, first, last); };
        boost::apply_visitor(annotate, ast);
    }

    template <typename ID, typename Iterator, typename Context>
    inline void
    on_success(ID, Iterator const& first, Iterator const& last, assignment& ast, Context const& context)
    {
        auto& pos_cache = x3::get<position_cache_tag>(context);
        pos_cache.annotate(ast, first, last);
    }
}}

#endif
