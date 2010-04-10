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
#include <boost/bind.hpp>

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
    struct value
    {
        utree val;
        value(utree const& val) : val(val) {}

        typedef utree result_type;
        utree operator()(utree const& /*args*/) const
        {
            return utree(boost::ref(val));
        }
    };

    struct value_composer
    {
        typedef function result_type;
        function operator()(utree const& val) const
        {
            return function(value(val));
        }
    };

    value_composer const val = {};

    ///////////////////////////////////////////////////////////////////////////
    // arguments
    ///////////////////////////////////////////////////////////////////////////
    struct argument
    {
        int n;
        argument(int n) : n(n) {}

        typedef utree result_type;
        utree operator()(utree const& args) const
        {
            return utree(boost::ref(args[n]));
        }
    };

    struct argument_composer
    {
        typedef function result_type;
        function operator()(int n) const
        {
            return function(argument(n));
        }
    };

    argument_composer const arg = {};

    ///////////////////////////////////////////////////////////////////////////
    // composite
    ///////////////////////////////////////////////////////////////////////////
    struct composite
    {
        function_list elements;
        composite(function_list const& elements)
          : elements(elements) {}
    };

    ///////////////////////////////////////////////////////////////////////////
    // plus
    ///////////////////////////////////////////////////////////////////////////
    struct plus_function : composite
    {
        plus_function(function_list const& elements)
          : composite(elements) {}

        typedef utree result_type;
        utree operator()(utree const& args) const
        {
            utree result(0);
            BOOST_FOREACH(function const& element, elements)
            {
                result = result + element(args);
            }
            return result;
        }
    };

    struct plus_composer
    {
        typedef function result_type;
        function operator()(function_list& elements)
        {
            return function(plus_function(elements));
        }
    };

    plus_composer const plus = {};

    ///////////////////////////////////////////////////////////////////////////
    // call
    ///////////////////////////////////////////////////////////////////////////
    struct call_function : composite
    {
        function f;
        call_function(function const& f, function_list const& elements)
          : composite(elements), f(f) {}

        typedef utree result_type;
        utree operator()(utree const& args) const
        {
            utree fargs;
            BOOST_FOREACH(function const& operand, elements)
            {
                fargs.push_back(operand(args));
            }
            return f(fargs);
        }
    };

    struct call_composer
    {
        function f;
        int arity;
        call_composer(function const& f, int arity)
          : f(f), arity(arity) {}

        typedef function result_type;
        function operator()(function_list& elements)
        {
            // $$$ use throw $$$
            BOOST_ASSERT(elements.size() == arity);
            return function(call_function(f, elements));
        }
    };

    inline call_composer const call(function const& f, int arity)
    {
        return call_composer(f, arity);
    }

///////////////////////////////////////////////////////////////////////////////
//  The compiler
///////////////////////////////////////////////////////////////////////////////
    typedef boost::function<function(function_list&)> function_compiler;

    struct compiler_environment
    {
        compiler_environment(compiler_environment* parent = 0)
          : parent(parent) {}

        void define(std::string const& name, function_compiler const& def)
        {
            // $$$ use exceptions here $$$
            BOOST_ASSERT(definitions.find(name) == definitions.end());
            definitions[name] = def;
        }

        function_compiler* find(std::string const& name)
        {
            std::map<std::string, function_compiler>::iterator
                i = definitions.find(name);
            if (i != definitions.end())
                return &i->second;
            else if (parent != 0)
                return parent->find(name);
            return 0;
        }

        compiler_environment* parent;
        std::map<std::string, function_compiler> definitions;
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

            if (function_compiler* mf = env.find(name))
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
                local_env.define(args[i], boost::bind(arg, i));
            env.define(name,
                function_compiler(call(compile(body, local_env), args.size())));
        }

        void define_nullary_function(
            std::string const& name,
            utree const& body) const
        {
            env.define(name,
                function_compiler(call(compile(body, env), 0)));
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

            if (function_compiler* mf = env.find(name))
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
        env.define("+", plus_composer());
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


