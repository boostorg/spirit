/*=============================================================================
    Copyright (c) 2001-2010 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_SCHEME_COMPILER)
#define BOOST_SPIRIT_SCHEME_COMPILER

#include <vector>
#include <map>
#include <boost/bind.hpp>

#include "scheme_intinsics.hpp"
#include "scheme_interpreter.hpp"

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

    struct external_function : composite<external_function>
    {
        // we must hold f by reference because functions can be recursive
        boost::reference_wrapper<actor const> f;

        external_function(actor const& f)
          : f(f) {}

        using base_type::operator();
        actor operator()(actor_list const& elements) const
        {
            return actor(lambda_function(f, elements));
        }
    };

///////////////////////////////////////////////////////////////////////////////
//  The compiler
///////////////////////////////////////////////////////////////////////////////
    class environment
    {
    public:

        environment(environment* parent = 0)
          : outer(parent) {}

        void define(std::string const& name, function_composer const& def)
        {
            // $$$ use exceptions here? $$$
            BOOST_ASSERT(definitions.find(name) == definitions.end());
            definitions[name] = def;
        }

        function_composer* find(std::string const& name)
        {
            std::map<std::string, function_composer>::iterator
                i = definitions.find(name);
            if (i != definitions.end())
                return &i->second;
            else if (outer != 0)
                return outer->find(name);
            return 0;
        }

        environment* parent() const { return outer; }

    private:

        environment* outer;
        std::map<std::string, function_composer> definitions;
    };

    actor compile(utree const& ast, environment& env, actor_list& fragments);

    struct compiler
    {
        typedef actor result_type;

        environment& env;
        actor_list& fragments;
        compiler(environment& env, actor_list& fragments)
          : env(env), fragments(fragments)
        {
        }

        actor operator()(nil) const
        {
            return scheme::val(utree());
        }

        template <typename T>
        actor operator()(T const& val) const
        {
            return scheme::val(utree(val));
        }

        actor operator()(utf8_symbol_range const& str) const
        {
            std::string name(str.begin(), str.end());
            if (function_composer* mf = env.find(name))
            {
                actor_list flist;
                return (*mf)(flist);
            }
            // $$$ throw? $$$
            return actor();
        }

        void define_function(
            std::string const& name,
            std::vector<std::string> const& args,
            utree const& body) const
        {
            environment local_env(&this->env);
            for (std::size_t i = 0; i < args.size(); ++i)
            {
                boost::function<actor(actor_list const&)>
                    f = boost::bind(arg, i);
                local_env.define(args[i], f);
            }

            fragments.push_back(actor());
            actor& f = fragments.back();
            env.define(name, function_composer(external_function(f)));
            f = compile(body, local_env, fragments);
        }

        void define_nullary_function(
            std::string const& name,
            utree const& body) const
        {
            fragments.push_back(actor());
            actor& f = fragments.back();
            env.define(name, function_composer(external_function(f)));
            f = compile(body, env, fragments);
        }

        template <typename Iterator>
        actor operator()(boost::iterator_range<Iterator> const& range) const
        {
            std::string name(get_symbol(*range.begin()));
            std::string fname;

            if (name == "define")
            {
                Iterator i = range.begin(); ++i;
                if (i->which() == utree_type::list_type)
                {
                    // a function
                    utree const& decl = *i++;
                    Iterator di = decl.begin();
                    fname = get_symbol(*di++);
                    std::vector<std::string> args;
                    while (di != decl.end())
                        args.push_back(get_symbol(*di++));
                    define_function(fname, args, *i);
                }
                else
                {
                    // constants (nullary functions)
                    fname = get_symbol(*i++);
                    define_nullary_function(fname, *i);
                }
                return actor(val(utf8_symbol("<define " + fname + ">")));
            }

            if (function_composer* mf = env.find(name))
            {
                actor_list flist;
                Iterator i = range.begin(); ++i;
                for (; i != range.end(); ++i)
                    flist.push_back(compile(*i, env, fragments));
                return (*mf)(flist);
            }

            return actor(); // $$$ implement me $$$
        }
    };

    actor compile(utree const& ast, environment& env, actor_list& fragments)
    {
        return utree::visit(ast, compiler(env, fragments));
    }

    void compile_all(
        utree const& ast,
        environment& env,
        actor_list& results,
        actor_list& fragments)
    {
        BOOST_FOREACH(utree const& program, ast)
        {
            scheme::actor f = compile(program, env, fragments);
            results.push_back(f);
        }
    }

    void build_basic_environment(environment& env)
    {
        env.define("if", if_);
        env.define("<=", less_than_equal);
        env.define("+", plus);
        env.define("-", minus);
        env.define("*", times);
    }
}

#endif
