/*=============================================================================
    Copyright (c) 2001-2010 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_SCHEME_INTERPRETER)
#define BOOST_SPIRIT_SCHEME_INTERPRETERnamespace scheme

#include <vector>
#include <map>
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
//  The compiler
///////////////////////////////////////////////////////////////////////////////
    typedef boost::function<function(function_list&)> function_compiler;

    class compiler_environment
    {
    public:

        compiler_environment(compiler_environment* parent = 0)
          : outer(parent) {}

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
            else if (outer != 0)
                return outer->find(name);
            return 0;
        }

        compiler_environment* parent() const { return outer; }

    private:

        compiler_environment* outer;
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

#endif
