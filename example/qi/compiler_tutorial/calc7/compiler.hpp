/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_CALC7_COMPILER_HPP)
#define BOOST_SPIRIT_CALC7_COMPILER_HPP

#include "ast.hpp"
#include <vector>
#include <map>

namespace client
{
    ///////////////////////////////////////////////////////////////////////////
    //  The Program
    ///////////////////////////////////////////////////////////////////////////
    struct program
    {
        void op(int a);
        void op(int a, int b);
        void op(int a, int b, int c);

        int& operator[](std::size_t i) { return code[i]; }
        int const& operator[](std::size_t i) const { return code[i]; }
        void clear() { code.clear(); variables.clear(); }
        std::vector<int> const& operator()() const { return code; }

        int nvars() const { return variables.size(); }
        int const* find_var(std::string const& name) const;
        void add_var(std::string const& name);

        void print_variables(std::vector<int> const& stack) const;

    private:

        std::map<std::string, int> variables;
        std::vector<int> code;
    };

    ///////////////////////////////////////////////////////////////////////////
    //  The Compiler
    ///////////////////////////////////////////////////////////////////////////
    struct compiler
    {
        typedef void result_type;

        compiler(client::program& program)
          : program(program) {}

        void operator()(ast::nil) const { BOOST_ASSERT(0); }
        void operator()(unsigned int x) const;
        void operator()(ast::variable const& x) const;
        void operator()(ast::operation const& x) const;
        void operator()(ast::signed_ const& x) const;
        void operator()(ast::expression const& x) const;
        void operator()(ast::assignment const& x) const;
        void operator()(ast::variable_declaration const& x) const;
        void operator()(ast::statement const& x) const;

        client::program& program;
    };
}

#endif