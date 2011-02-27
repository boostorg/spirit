/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include "compiler.hpp"
#include "vm.hpp"
#include <boost/foreach.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/assert.hpp>
#include <iostream>
#include <string>

namespace client
{
    void program::op(int a)
    {
        code.push_back(a);
    }

    void program::op(int a, int b)
    {
        code.push_back(a);
        code.push_back(b);
    }

    void program::op(int a, int b, int c)
    {
        code.push_back(a);
        code.push_back(b);
        code.push_back(c);
    }

    int const* program::find_var(std::string const& name) const
    {
        std::map<std::string, int>::const_iterator i = variables.find(name);
        if (i == variables.end())
            return 0;
        return &i->second;
    }

    void program::add_var(std::string const& name)
    {
        variables[name] = variables.size();
    }

    void program::print_variables(std::vector<int> const& stack) const
    {
        typedef std::pair<std::string, int> pair;
        BOOST_FOREACH(pair const& p, variables)
        {
            std::cout << "    " << p.first << ": " << stack[p.second] << std::endl;
        }
    }

    void compiler::operator()(unsigned int x) const
    {
        program.op(op_int, x);
    }

    void compiler::operator()(ast::variable const& x) const
    {
        int const* p = program.find_var(x.name);
        if (p == 0)
        {
            // $$$ undefined variable throw exception
            std::cout << "undefined variable: " << x.name << std::endl;
        }
        program.op(op_load, *p);
    }

    void compiler::operator()(ast::operation const& x) const
    {
        boost::apply_visitor(*this, x.operand_);
        switch (x.operator_)
        {
            case '+': program.op(op_add); break;
            case '-': program.op(op_sub); break;
            case '*': program.op(op_mul); break;
            case '/': program.op(op_div); break;
            default: BOOST_ASSERT(0); break;
        }
    }

    void compiler::operator()(ast::signed_ const& x) const
    {
        boost::apply_visitor(*this, x.operand_);
        switch (x.sign)
        {
            case '-': program.op(op_neg); break;
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

    void compiler::operator()(ast::assignment const& x) const
    {
        (*this)(x.rhs);
        int const* p = program.find_var(x.lhs.name);
        if (p == 0)
        {
            // $$$ undefined variable throw exception
            std::cout << "undefined variable: " << x.lhs.name << std::endl;
        }
        program.op(op_store, *p);
    }

    void compiler::operator()(ast::variable_declaration const& x) const
    {
        int const* p = program.find_var(x.assign.lhs.name);
        if (p != 0)
        {
            // $$$ duplicate variable throw exception
            std::cout << "duplicate variable: " << x.assign.lhs.name << std::endl;
        }
        program.add_var(x.assign.lhs.name);
        (*this)(x.assign);
    }

    void compiler::operator()(ast::statement const& x) const
    {
        typedef
            boost::variant<
                ast::variable_declaration,
                ast::assignment>
        statement;

        try
        {
            program.clear();

            // op_adstk 0 for now. we'll know how many variables we'll have later
            program.op(op_adstk, 0);
            BOOST_FOREACH(statement const& s, x)
            {
                boost::apply_visitor(*this, s);
            }
            program[1] = program.nvars(); // now store the actual number of variables
        }
        catch(...)
        {
            program.clear();
        }
    }
}

