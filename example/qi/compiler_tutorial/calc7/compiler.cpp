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

    bool compiler::operator()(unsigned int x) const
    {
        program.op(op_int, x);
        return true;
    }

    bool compiler::operator()(ast::variable const& x) const
    {
        int const* p = program.find_var(x.name);
        if (p == 0)
        {
            std::cout << x.id << std::endl;
            error_handler(x.id, "Undeclared variable: " + x.name);
            return false;
        }
        program.op(op_load, *p);
        return true;
    }

    bool compiler::operator()(ast::operation const& x) const
    {
        if (!boost::apply_visitor(*this, x.operand_))
            return false;
        switch (x.operator_)
        {
            case '+': program.op(op_add); break;
            case '-': program.op(op_sub); break;
            case '*': program.op(op_mul); break;
            case '/': program.op(op_div); break;
            default: BOOST_ASSERT(0); return false;
        }
        return true;
    }

    bool compiler::operator()(ast::signed_ const& x) const
    {
        if (!boost::apply_visitor(*this, x.operand_))
            return false;
        switch (x.sign)
        {
            case '-': program.op(op_neg); break;
            case '+': break;
            default: BOOST_ASSERT(0); return false;
        }
        return true;
    }

    bool compiler::operator()(ast::expression const& x) const
    {
        if (!boost::apply_visitor(*this, x.first))
            return false;
        BOOST_FOREACH(ast::operation const& oper, x.rest)
        {
            if (!(*this)(oper))
                return false;
        }
        return true;
    }

    bool compiler::operator()(ast::assignment const& x) const
    {
        if (!(*this)(x.rhs))
            return false;
        int const* p = program.find_var(x.lhs.name);
        if (p == 0)
        {
            std::cout << x.lhs.id << std::endl;
            error_handler(x.lhs.id, "Undeclared variable: " + x.lhs.name);
            return false;
        }
        program.op(op_store, *p);
        return true;
    }

    bool compiler::operator()(ast::variable_declaration const& x) const
    {
        int const* p = program.find_var(x.assign.lhs.name);
        if (p != 0)
        {
            std::cout << x.assign.lhs.id << std::endl;
            error_handler(x.assign.lhs.id, "Duplicate variable: " + x.assign.lhs.name);
            return false;
        }
        bool r = (*this)(x.assign.rhs);
        if (r) // don't add the variable if the RHS fails
        {
            program.add_var(x.assign.lhs.name);
            program.op(op_store, *program.find_var(x.assign.lhs.name));
        }
        return r;
    }

    bool compiler::operator()(ast::statement const& x) const
    {
        typedef
            boost::variant<
                ast::variable_declaration,
                ast::assignment>
        statement;

        program.clear();

        // op_adstk 0 for now. we'll know how many variables we'll have later
        program.op(op_adstk, 0);
        BOOST_FOREACH(statement const& s, x)
        {
            if (!boost::apply_visitor(*this, s))
            {
                program.clear();
                return false;
            }
        }
        program[1] = program.nvars(); // now store the actual number of variables
        return true;
    }
}

