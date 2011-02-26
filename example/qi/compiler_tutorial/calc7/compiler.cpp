/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include "compiler.hpp"
#include "vm.hpp"
#include <boost/foreach.hpp>
#include <boost/variant/apply_visitor.hpp>

namespace client
{
    void compiler::operator()(unsigned int n) const
    {
        op(op_int);
        op(n);
    }

    void compiler::operator()(ast::operand const& x) const
    {
        boost::apply_visitor(*this, x);
    }

    void compiler::operator()(ast::operation const& x) const
    {
        (*this)(x.operand_);
        switch (x.operator_)
        {
            case '+': op(op_add); break;
            case '-': op(op_sub); break;
            case '*': op(op_mul); break;
            case '/': op(op_div); break;
            default: BOOST_ASSERT(0); break;
        }
    }

    void compiler::operator()(ast::signed_ const& x) const
    {
        (*this)(x.operand_);
        switch (x.sign)
        {
            case '-': op(op_neg); break;
            case '+': break;
            default: BOOST_ASSERT(0); break;
        }
    }

    void compiler::operator()(ast::program const& x) const
    {
        (*this)(x.first);
        BOOST_FOREACH(ast::operation const& oper, x.rest)
        {
            (*this)(oper);
        }
    }

    void compiler::operator()(ast::tagged const& x) const
    {
        (*this)(x.operand_);
    }
}

