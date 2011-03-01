/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_CALC8_COMPILER_HPP)
#define BOOST_SPIRIT_CALC8_COMPILER_HPP

#include "ast.hpp"
#include "error_handler.hpp"
#include <vector>
#include <map>
#include <set>
#include <boost/function.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_function.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

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
        void clear() { code.clear(); variables.clear(); jumps.clear(); }
        std::size_t size() const { return code.size(); }
        std::vector<int> const& operator()() const { return code; }

        int nvars() const { return variables.size(); }
        int const* find_var(std::string const& name) const;
        void add_var(std::string const& name);

        void print_variables(std::vector<int> const& stack) const;
        void print_assembler() const;

        void add_jump(std::size_t jump) { jumps.insert(jump); }

    private:

        std::map<std::string, int> variables;
        std::set<std::size_t> jumps;
        std::vector<int> code;
    };

    ///////////////////////////////////////////////////////////////////////////
    //  The Compiler
    ///////////////////////////////////////////////////////////////////////////
    struct compiler
    {
        typedef bool result_type;

        template <typename ErrorHandler>
        compiler(client::program& program, ErrorHandler& error_handler_)
          : program(program)
        {
            using namespace boost::phoenix::arg_names;
            using boost::phoenix::cref;
            using boost::phoenix::function;

            error_handler = function<ErrorHandler>(error_handler_)(
                "Error! ", _2, cref(error_handler_.iters)[_1]);
        }

        bool operator()(ast::nil) const { BOOST_ASSERT(0); return false; }
        bool operator()(unsigned int x) const;
        bool operator()(bool x) const;
        bool operator()(ast::variable const& x) const;
        bool operator()(ast::operation const& x) const;
        bool operator()(ast::unary const& x) const;
        bool operator()(ast::expression const& x) const;
        bool operator()(ast::assignment const& x) const;
        bool operator()(ast::variable_declaration const& x) const;
        bool operator()(ast::statement_list const& x) const;
        bool operator()(ast::statement const& x) const;
        bool operator()(ast::if_statement const& x) const;
        bool operator()(ast::while_statement const& x) const;

        bool start(ast::statement_list const& x) const;

        client::program& program;

        boost::function<
            void(int tag, std::string const& what)>
        error_handler;
    };
}

#endif