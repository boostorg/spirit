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

    void program::print_assembler() const
    {
        std::vector<int>::const_iterator pc = code.begin();

        std::vector<std::string> locals(variables.size());
        typedef std::pair<std::string, int> pair;
        BOOST_FOREACH(pair const& p, variables)
        {
            locals[p.second] = p.first;
            std::cout << "      local       "
                << p.first << ", @" << p.second << std::endl;
        }

        std::cout << "start:" << std::endl;
        while (pc != code.end())
        {
            std::size_t pos = pc-code.begin();
            if (jumps.find(pos) != jumps.end())
                std::cout << pos << ':' << std::endl;

            switch (*pc++)
            {
                case op_neg:
                    std::cout << "      op_neg" << std::endl;
                    break;

                case op_not:
                    std::cout << "      op_not" << std::endl;
                    break;

                case op_add:
                    std::cout << "      op_add" << std::endl;
                    break;

                case op_sub:
                    std::cout << "      op_sub" << std::endl;
                    break;

                case op_mul:
                    std::cout << "      op_mul" << std::endl;
                    break;

                case op_div:
                    std::cout << "      op_div" << std::endl;
                    break;

                case op_eq:
                    std::cout << "      op_eq" << std::endl;
                    break;

                case op_neq:
                    std::cout << "      op_neq" << std::endl;
                    break;

                case op_lt:
                    std::cout << "      op_lt" << std::endl;
                    break;

                case op_lte:
                    std::cout << "      op_lte" << std::endl;
                    break;

                case op_gt:
                    std::cout << "      op_gt" << std::endl;
                    break;

                case op_gte:
                    std::cout << "      op_gte" << std::endl;
                    break;

                case op_and:
                    std::cout << "      op_and" << std::endl;
                    break;

                case op_or:
                    std::cout << "      op_or" << std::endl;
                    break;

                case op_load:
                    std::cout << "      op_load     " << locals[*pc++] << std::endl;
                    break;

                case op_store:
                    std::cout << "      op_store    " << locals[*pc++] << std::endl;
                    break;

                case op_int:
                    std::cout << "      op_int      " << *pc++ << std::endl;
                    break;

                case op_true:
                    std::cout << "      op_true" << std::endl;
                    break;

                case op_false:
                    std::cout << "      op_false" << std::endl;
                    break;

                case op_jump:
                    std::cout << "      op_jump     " << *pc++ << std::endl;
                    break;

                case op_jump_if:
                    std::cout << "      op_jump_if  " << *pc++ << std::endl;
                    break;

                case op_adstk:
                    std::cout << "      op_adstk    " << *pc++ << std::endl;
                    break;
            }
        }

        if (jumps.find(code.size()) != jumps.end())
            std::cout << code.size() << ':' << std::endl;
    }

    bool compiler::operator()(unsigned int x) const
    {
        program.op(op_int, x);
        return true;
    }

    bool compiler::operator()(bool x) const
    {
        program.op(x ? op_true : op_false);
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
            case ast::op_plus: program.op(op_add); break;
            case ast::op_minus: program.op(op_sub); break;
            case ast::op_times: program.op(op_mul); break;
            case ast::op_divide: program.op(op_div); break;

            case ast::op_equal: program.op(op_eq); break;
            case ast::op_not_equal: program.op(op_neq); break;
            case ast::op_less: program.op(op_lt); break;
            case ast::op_less_equal: program.op(op_lte); break;
            case ast::op_greater: program.op(op_gt); break;
            case ast::op_greater_equal: program.op(op_gte); break;

            case ast::op_and: program.op(op_and); break;
            case ast::op_or: program.op(op_or); break;
            default: BOOST_ASSERT(0); return false;
        }
        return true;
    }

    bool compiler::operator()(ast::unary const& x) const
    {
        if (!boost::apply_visitor(*this, x.operand_))
            return false;
        switch (x.operator_)
        {
            case ast::op_negative: program.op(op_neg); break;
            case ast::op_not: program.op(op_not); break;
            case ast::op_positive: break;
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
        return boost::apply_visitor(*this, x);
    }

    bool compiler::operator()(ast::statement_list const& x) const
    {
        BOOST_FOREACH(ast::statement const& s, x)
        {
            if (!(*this)(s))
                return false;
        }
        return true;
    }

    bool compiler::operator()(ast::if_statement const& x) const
    {
        if (!(*this)(x.condition))
            return false;
        program.op(op_jump_if, 0);              // we shall fill this (0) in later
        std::size_t skip = program.size()-1;    // mark its position
        if (!(*this)(x.then))
            return false;
        program[skip] = program.size();         // now we know where to jump to (after the if branch)

        if (x.else_)                            // We got an alse
        {
            program[skip] += 2;                 // adjust for the "else" jump
            program.op(op_jump, 0);             // we shall fill this (0) in later
            std::size_t exit = program.size()-1;// mark its position
            if (!(*this)(*x.else_))
                return false;
            program[exit] = program.size();      // now we know where to jump to (after the else branch)
            program.add_jump(program[exit]);
        }

        program.add_jump(program[skip]);
        return true;
    }

    bool compiler::operator()(ast::while_statement const& x) const
    {
        std::size_t loop = program.size();      // mark our position
        if (!(*this)(x.condition))
            return false;
        program.op(op_jump_if, 0);              // we shall fill this (0) in later
        std::size_t exit = program.size()-1;    // mark its position
        if (!(*this)(x.body))
            return false;
        program.op(op_jump, loop);              // loop back
        program[exit] = program.size();         // now we know where to jump to (to exit the loop)

        program.add_jump(loop);
        program.add_jump(program[exit]);
        return true;
    }

    bool compiler::start(ast::statement_list const& x) const
    {
        program.clear();
        // op_adstk 0 for now. we'll know how many variables we'll have later
        program.op(op_adstk, 0);

        if (!(*this)(x))
        {
            program.clear();
            return false;
        }
        program[1] = program.nvars(); // now store the actual number of variables
        return true;
    }
}

