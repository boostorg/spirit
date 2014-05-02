/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_X3_CALC7_COMPILER_HPP)
#define BOOST_SPIRIT_X3_CALC7_COMPILER_HPP

#include "ast.hpp"
#include "vm.hpp"

namespace client
{
    ///////////////////////////////////////////////////////////////////////////
    //  The Compiler
    ///////////////////////////////////////////////////////////////////////////
    struct compiler
    {
        typedef void result_type;

        std::vector<int>& code;
        compiler(std::vector<int>& code)
          : code(code) {}

        void operator()(ast::nil) const { BOOST_ASSERT(0); }
        void operator()(unsigned int n) const
        {
            code.push_back(op_int);
            code.push_back(n);
        }

        void operator()(ast::operation const& x) const
        {
            boost::apply_visitor(*this, x.operand_);
            switch (x.operator_)
            {
                case '+': code.push_back(op_add); break;
                case '-': code.push_back(op_sub); break;
                case '*': code.push_back(op_mul); break;
                case '/': code.push_back(op_div); break;
                default: BOOST_ASSERT(0); break;
            }
        }

        void operator()(ast::signed_ const& x) const
        {
            boost::apply_visitor(*this, x.operand_);
            switch (x.sign)
            {
                case '-': code.push_back(op_neg); break;
                case '+': break;
                default: BOOST_ASSERT(0); break;
            }
        }

        void operator()(ast::expression const& x) const
        {
            boost::apply_visitor(*this, x.first);
            for (ast::operation const& oper : x.rest)
            {
                (*this)(oper);
            }
        }
    };
}

#endif
