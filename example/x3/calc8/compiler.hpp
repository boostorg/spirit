/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_X3_CALC8_COMPILER_HPP)
#define BOOST_SPIRIT_X3_CALC8_COMPILER_HPP

#include "ast.hpp"

namespace client
{
    ///////////////////////////////////////////////////////////////////////////
    //  The Compiler
    ///////////////////////////////////////////////////////////////////////////
    struct compiler
    {
        typedef bool result_type;

        std::vector<int>& code;
        compiler(std::vector<int>& code)
          : code(code) {}

        bool operator()(ast::nil) const;
        bool operator()(unsigned int n) const;
        bool operator()(ast::variable const& x) const;
        bool operator()(ast::operation const& x) const;
        bool operator()(ast::signed_ const& x) const;
        bool operator()(ast::expression const& x) const;
    };
}

#endif
