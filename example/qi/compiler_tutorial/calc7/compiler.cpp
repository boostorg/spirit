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
    void compiler::op(int a) const
    {
        code.push_back(a);
    }

    void compiler::op(int a, int b) const
    {
        code.push_back(a);
        code.push_back(b);
    }

    void compiler::op(int a, int b, int c) const
    {
        code.push_back(a);
        code.push_back(b);
        code.push_back(c);
    }

    void compiler::operator()(unsigned int x) const
    {
        op(op_int, x);
    }

    void compiler::operator()(ast::variable const& x) const
    {
        // $$$ TODO $$$
        op(op_int, 555); // for now
    }

    void compiler::operator()(ast::operation const& x) const
    {
        boost::apply_visitor(*this, x.operand_);
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
        boost::apply_visitor(*this, x.operand_);
        switch (x.sign)
        {
            case '-': op(op_neg); break;
            case '+': break;
            default: BOOST_ASSERT(0); break;
        }
    }

    void compiler::operator()(ast::expression const& x) const
    {
        boost::apply_visitor(*this, x.first);
        BOOST_FOREACH(ast::operation const& oper, x.rest)
        {
            (*this)(oper);
        }
    }
}

