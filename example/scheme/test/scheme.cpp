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
//  Utilities
///////////////////////////////////////////////////////////////////////////////
    inline std::string get_symbol(utree const& s)
    {
        utf8_symbol_range symbol = s.as<utf8_symbol_range>();
        return std::string(symbol.begin(), symbol.end());
    }

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

    struct make_arg
    {
        int n;
        make_arg(int n) : n(n) {}

        typedef function result_type;
        function operator()(function_list& operands)
        {
            // check arity here!
            return function(argument_impl(n));
        }
    };

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

    ///////////////////////////////////////////////////////////////////////////
    // fcall
    ///////////////////////////////////////////////////////////////////////////
    struct fcall_impl
    {
        function f;
        function_list operands;

        fcall_impl(function const& f, function_list& operands_)
          : f(f)
        {
            // we take ownership of the operands
            operands_.swap(operands);
        }

        typedef utree result_type;
        utree operator()(utree const& args) const
        {
            utree fargs;
            BOOST_FOREACH(function const& operand, operands)
            {
                fargs.push_back(operand(args));
            }
            return f(fargs);
        }
    };

    struct make_fcall
    {
        function f;
        int arity;
        make_fcall(function const& f, int arity)
          : f(f), arity(arity) {}

        typedef function result_type;
        function operator()(function_list& operands)
        {
            // $$$ use throw $$$
            BOOST_ASSERT(operands.size() == arity);
            return function(fcall_impl(f, operands));
        }
    };

///////////////////////////////////////////////////////////////////////////////
//  The compiler
///////////////////////////////////////////////////////////////////////////////
    typedef boost::function<function(function_list&)> make_function;

    struct compiler_environment
    {
        compiler_environment(compiler_environment* parent = 0)
          : parent(parent) {}

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
            else if (parent != 0)
                return parent->find(name);
            return 0;
        }

        compiler_environment* parent;
        std::map<std::string, make_function> definitions;
    };

    function compile(utree const& ast, compiler_environment& env);

    struct compiler
    {
        typedef function result_type;

        mutable compiler_environment& env;
        compiler(compiler_environment& env)
          : env(env)
        {
        }

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
            std::string name(str.begin(), str.end());

            if (make_function* mf = env.find(name))
            {
                function_list flist;
                return (*mf)(flist);
            }
            // $$$ throw? $$$
            return function();
        }

        void define_function(
            std::string const& name,
            std::vector<std::string> const& args,
            utree const& body) const
        {
            compiler_environment local_env(env);
            for (std::size_t i = 0; i < args.size(); ++i)
                local_env.define(args[i], make_arg(i));
            make_fcall mf(compile(body, local_env), args.size());
            env.define(name, make_function(mf));
        }

        void define_nullary_function(
            std::string const& name,
            utree const& body) const
        {
            make_fcall mf(compile(body, env), 0);
            env.define(name, make_function(mf));
        }

        template <typename Iterator>
        function operator()(boost::iterator_range<Iterator> const& range) const
        {
            std::string name(get_symbol(*range.begin()));

            if (name == "define")
            {
                Iterator i = range.begin(); ++i;
                if (i->which() == utree_type::list_type)
                {
                    // a function
                    utree const& decl = *i++;


                    Iterator di = decl.begin();
                    std::string fname(get_symbol(*di++));
                    std::vector<std::string> args;
                    while (di != decl.end())
                        args.push_back(get_symbol(*di++));
                    define_function(fname, args, *i);
                }
                else
                {
                    // constants (nullary functions)
                    std::string fname(get_symbol(*i++));
                    define_nullary_function(fname, *i);
                }
                return function(val(utree(utf8_symbol("<function>"))));
            }

            if (make_function* mf = env.find(name))
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

    void compile_all(
        utree const& ast,
        compiler_environment& env,
        function_list& results)
    {
        BOOST_FOREACH(utree const& program, ast)
        {
            scheme::function f = compile(program, env);
            results.push_back(f);
        }
    }

    void build_basic_environment(compiler_environment& env)
    {
        env.define("+", make_plus());
    }
}

///////////////////////////////////////////////////////////////////////////////
//  Main program
///////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
    char const* filename = NULL;
    if (argc > 1)
    {
        filename = argv[1];
    }
    else
    {
        std::cerr << "Error: No input file provided." << std::endl;
        return 1;
    }

    std::ifstream in(filename, std::ios_base::in);

    if (!in)
    {
        std::cerr << "Error: Could not open input file: "
            << filename << std::endl;
        return 1;
    }

    // Ignore the BOM marking the beginning of a UTF-8 file in Windows
    char c = in.peek();
    if (c == '\xef')
    {
        char s[3];
        in >> s[0] >> s[1] >> s[2];
        s[3] = '\0';
        if (s != std::string("\xef\xbb\xbf"))
        {
            std::cerr << "Error: Unexpected characters from input file: "
                << filename << std::endl;
            return 1;
        }
    }

    scheme::utree program;
    if (scheme::input::parse_sexpr_list(in, program))
    {
        std::cout << "success: " << std::endl;
        scheme::compiler_environment env;
        scheme::build_basic_environment(env);
        scheme::function_list flist;
        compile_all(program, env, flist);
        BOOST_FOREACH(scheme::function const& f, flist)
        {
            std::cout << " result: " << f(scheme::utree()) << std::endl;
        }
    }
    else
    {
        std::cout << "parse error" << std::endl;
    }

    return 0;
}


