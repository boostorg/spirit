/*=============================================================================
    Copyright (c) 2001-2010 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include <boost/config/warning_disable.hpp>

#include "../input/sexpr.hpp"
#include "../input/parse_sexpr_impl.hpp"
#include <iostream>
#include <fstream>

inline std::ostream& println(std::ostream& out, scheme::utree const& val)
{
    out << val << std::endl;
    return out;
}

#include <vector>
#include <list>
#include <map>
#include <boost/function.hpp>
#include <boost/foreach.hpp>

namespace scheme
{
///////////////////////////////////////////////////////////////////////////////
//  The runtime interpreter
///////////////////////////////////////////////////////////////////////////////

    typedef boost::function<utree(utree const& args)> function;
    typedef std::list<function> function_list;

    ///////////////////////////////////////////////////////////////////////////
    // values
    ///////////////////////////////////////////////////////////////////////////
    struct value_impl
    {
        utree val;
        value_impl(utree const& val) : val(val) {}

        typedef utree result_type;
        utree operator()(utree const& /*args*/) const
        {
            return utree(boost::ref(val));
        }
    };

    inline function val(utree const& val)
    {
        return function(value_impl(val));
    }

    ///////////////////////////////////////////////////////////////////////////
    // arguments
    ///////////////////////////////////////////////////////////////////////////
    struct argument_impl
    {
        int n;
        argument_impl(int n) : n(n) {}

        typedef utree result_type;
        utree operator()(utree const& args) const
        {
            return utree(boost::ref(args[n]));
        }
    };

    inline function arg(int n)
    {
        return function(argument_impl(n));
    }

    ///////////////////////////////////////////////////////////////////////////
    // reference
    ///////////////////////////////////////////////////////////////////////////
    struct reference_impl
    {
        function& f;
        reference_impl(function& f) : f(f) {}

        typedef utree result_type;
        utree operator()(utree const& args) const
        {
            return f(args);
        }
    };

    inline function ref(function& f)
    {
        return function(reference_impl(f));
    }

    ///////////////////////////////////////////////////////////////////////////
    // plus
    ///////////////////////////////////////////////////////////////////////////
    struct plus_impl
    {
        function_list operands;

        plus_impl(function_list& operands_)
        {
            // we take ownership of the operands
            operands_.swap(operands);
        }

        typedef utree result_type;
        utree operator()(utree const& args) const
        {
            utree result(0);
            BOOST_FOREACH(function const& operand, operands)
            {
                result = result + operand(args);
            }
            return result;
        }
    };

    struct make_plus
    {
        typedef function result_type;
        function operator()(function_list& operands)
        {
            // check arity here!
            return function(plus_impl(operands));
        }
    };

    make_plus const plus = {};

///////////////////////////////////////////////////////////////////////////////
//  The compiler
///////////////////////////////////////////////////////////////////////////////
    typedef boost::function<function(function_list&)> make_function;

    struct compiler_environment
    {
        compiler_environment() {}

        void define(std::string const& name, make_function const& def)
        {
            // $$$ use exceptions here $$$
            BOOST_ASSERT(definitions.find(name) == definitions.end());
            definitions[name] = def;
        }

        make_function* find(std::string const& name)
        {
            std::map<std::string, make_function>::iterator
                i = definitions.find(name);
            if (i != definitions.end())
                return &i->second;
            return 0;
        }

        std::map<std::string, make_function> definitions;
    };

    function compile(utree const& ast, compiler_environment& env);

    struct compiler
    {
        typedef function result_type;

        compiler_environment& env;
        compiler(compiler_environment& env)
          : env(env) {}

        function operator()(nil) const
        {
            return scheme::val(utree());
        }

        template <typename T>
        function operator()(T const& val) const
        {
            return scheme::val(utree(val));
        }

        function operator()(utf8_symbol_range const& str) const
        {
            return function(); // $$$ implement me $$$
        }

        template <typename Iterator>
        function operator()(boost::iterator_range<Iterator> const& range) const
        {
            utf8_symbol_range symbol = range.begin()->as<utf8_symbol_range>();
            std::string fname(symbol.begin(), symbol.end());
            if (make_function* mf = env.find(fname))
            {
                function_list flist;
                Iterator i = range.begin(); ++i;
                for (; i != range.end(); ++i)
                    flist.push_back(compile(*i, env));
                return (*mf)(flist);
            }

            return function(); // $$$ implement me $$$
        }
    };

    function compile(utree const& ast, compiler_environment& env)
    {
        return utree::visit(ast, compiler(env));
    }

    void build_basic_environment(compiler_environment& env)
    {
        env.define("+", make_plus());
    }
}

///////////////////////////////////////////////////////////////////////////////
//  Main program
///////////////////////////////////////////////////////////////////////////////
int main()
{
    std::string in;
    while (std::getline(std::cin, in))
    {
        if (in.empty() || in[0] == 'q' || in[0] == 'Q')
            break;

        typedef std::string::const_iterator iterator_type;
        scheme::utree result;
        scheme::input::sexpr<iterator_type> p;
        scheme::input::white_space<iterator_type> ws;

        iterator_type first = in.begin();
        iterator_type last = in.end();
        if (phrase_parse(first, last, p, ws, result))
        {
            std::cout << "success: ";
            scheme::compiler_environment env;
            scheme::build_basic_environment(env);
            scheme::function f = compile(result, env);
            std::cout << "result: " << f(scheme::utree()) << std::endl;
        }
        else
        {
            std::cout << "parse error" << std::endl;
        }
    }

    std::cout << "Bye... :-) \n\n";
    return 0;
}


