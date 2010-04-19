/*=============================================================================
    Copyright (c) 2001-2010 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_SCHEME_COMPILER)
#define BOOST_SPIRIT_SCHEME_COMPILER

#include <vector>
#include <map>
#include <exception>

#include <boost/bind.hpp>
#include <scheme/intrinsics.hpp>
#include <scheme/interpreter.hpp>
#include <input/parse_sexpr.hpp>

namespace scheme
{
///////////////////////////////////////////////////////////////////////////////
//  Exceptions
///////////////////////////////////////////////////////////////////////////////
    struct scheme_exception : std::exception {};

    struct compilation_error : std::exception
    {
        virtual const char* what() const throw()
        {
            return "scheme: Compilation error";
        }
    };

    struct identifier_expected : scheme_exception
    {
        virtual const char* what() const throw()
        {
            return "scheme: Identifier expected";
        }
    };

    struct identifier_not_found : scheme_exception
    {
        std::string msg;
        identifier_not_found(std::string const& id)
          : msg("scheme: Identifier (" + id + ") not found") {}

        virtual const char* what() const throw()
        {
            return msg.c_str();;
        }
    };

    struct duplicate_identifier : scheme_exception
    {
        std::string msg;
        duplicate_identifier(std::string const& id)
          : msg("scheme: Duplicate identifier (" + id + ')') {}

        virtual const char* what() const throw()
        {
            return "scheme: Duplicate identifier";
        }
    };

    struct incorrect_arity : scheme_exception
    {
        virtual const char* what() const throw()
        {
            return "scheme: Invalid number of parameters to function call";
        }
    };

///////////////////////////////////////////////////////////////////////////////
//  The environment
///////////////////////////////////////////////////////////////////////////////
    typedef boost::function<function(actor_list const&)> compiled_function;

    class environment
    {
    public:

        environment(environment* parent = 0)
          : outer(parent) {}

        template <typename Function>
        void define(std::string const& name, Function const& f, int arity)
        {
            if (definitions.find(name) != definitions.end())
                throw duplicate_identifier(name);
            definitions[name] = std::make_pair(compiled_function(f), arity);
        }

        std::pair<compiled_function*, int>
        find(std::string const& name)
        {
            std::map<std::string, map_element>::iterator
                i = definitions.find(name);
            if (i != definitions.end())
                return std::make_pair(&i->second.first, i->second.second);
            else if (outer != 0)
                return outer->find(name);
            return std::make_pair((compiled_function*)0, 0);
        }

        void undefine(std::string const& name)
        {
            definitions.erase(name);
        }

        bool defined(std::string const& name)
        {
            return definitions.find(name) != definitions.end();
        }

        environment* parent() const { return outer; }

    private:

        typedef std::pair<compiled_function, int> map_element;

        environment* outer;
        std::map<std::string, map_element> definitions;
    };

///////////////////////////////////////////////////////////////////////////////
//  The compiler
///////////////////////////////////////////////////////////////////////////////
    function compile(
        utree const& ast,
        environment& env,
        actor_list& fragments,
        int parent_line,
        std::string const& source_file = "");

    struct external_function : composite<external_function>
    {
        // we must hold f by reference because functions can be recursive
        boost::reference_wrapper<function const> f;

        external_function(function const& f)
          : f(f) {}

        using base_type::operator();
        function operator()(actor_list const& elements) const
        {
            return function(lambda_function(f, elements));
        }
    };

    struct compiler
    {
        typedef function result_type;
        environment& env;
        actor_list& fragments;
        int line;
        std::string source_file;

        compiler(
            environment& env,
            actor_list& fragments,
            int line,
            std::string const& source_file = "")
          : env(env), fragments(fragments),
            line(line), source_file(source_file)
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
            std::pair<compiled_function*, int> r = env.find(name);
            if (r.first)
            {
                actor_list flist;
                return (*r.first)(flist);
            }
            throw identifier_not_found(name);
            return function();
        }

        function make_lambda(
            std::vector<std::string> const& args,
            utree const& body) const
        {
            environment local_env(&this->env);
            for (std::size_t i = 0; i < args.size(); ++i)
                local_env.define(args[i], boost::bind(arg, i), args.size());
            return compile(body, local_env, fragments, line, source_file);
        }

        function define_function(
            std::string const& name,
            std::vector<std::string> const& args,
            utree const& body) const
        {
            try
            {
                if (env.defined(name))
                    throw duplicate_identifier(name);
                fragments.push_back(function());
                function& f = fragments.back();
                env.define(name, external_function(f), args.size());
                f = make_lambda(args, body);
                return f;
            }
            catch (compilation_error const&)
            {
                env.undefine(name);
                throw;
            }
        }

        template <typename Iterator>
        function operator()(boost::iterator_range<Iterator> const& range) const
        {
            std::string name(get_symbol(*range.begin()));

            if (name == "define")
            {
                std::string fname;
                std::vector<std::string> args;

                Iterator i = range.begin(); ++i;
                if (i->which() == utree_type::list_type)
                {
                    // (define (f x) ...body...)
                    utree const& decl = *i++;
                    Iterator di = decl.begin();
                    fname = get_symbol(*di++);
                    while (di != decl.end())
                        args.push_back(get_symbol(*di++));
                }
                else
                {
                    // (define f ...body...)
                    fname = get_symbol(*i++);

                    // (define f (lambda (x) ...body...))
                    if (i->which() == utree_type::list_type
                        && get_symbol((*i)[0]) == "lambda")
                    {
                        utree const& arg_names = (*i)[1];
                        Iterator ai = arg_names.begin();
                        while (ai != arg_names.end())
                            args.push_back(get_symbol(*ai++));
                    }
                }

                return define_function(fname, args, *i);
            }

            if (name == "lambda")
            {
                // (lambda (x) ...body...)
                Iterator i = range.begin(); ++i;
                utree const& arg_names = *i++;
                Iterator ai = arg_names.begin();
                std::vector<std::string> args;
                while (ai != arg_names.end())
                    args.push_back(get_symbol(*ai++));
                return make_lambda(args, *i);
            }

            // (f x)
            std::pair<compiled_function*, int> r = env.find(name);
            if (r.first)
            {
                actor_list flist;
                Iterator i = range.begin(); ++i;
                for (; i != range.end(); ++i)
                    flist.push_back(
                        compile(*i, env, fragments, line, source_file));

                // Arity check
                if (r.second < 0) // non-fixed arity
                {
                    if (int(flist.size()) < -r.second)
                        throw incorrect_arity();
                }
                else // fixed arity
                {
                    if (int(flist.size()) != r.second)
                        throw incorrect_arity();
                }
                return (*r.first)(flist);
            }
            else
            {
                throw identifier_not_found(name);
            }

            // Just return the list $$$ TODO $$$ how do we disambiguate lists
            // and function calls?
            return function();
        }

        static std::string get_symbol(utree const& s)
        {
            if (s.which() != utree_type::symbol_type)
                throw identifier_expected();
            utf8_symbol_range symbol = s.as<utf8_symbol_range>();
            return std::string(symbol.begin(), symbol.end());
        }
    };

    inline function compile(
        utree const& ast,
        environment& env,
        actor_list& fragments,
        int parent_line,
        std::string const& source_file)
    {
        int line = (ast.which() == utree_type::list_type)
            ? ast.tag() : parent_line;

        try
        {
            return utree::visit(ast,
                compiler(env, fragments, line, source_file));
        }
        catch (scheme_exception const& x)
        {
            if (source_file != "")
                std::cerr << source_file;

            if (line != -1)
                std::cerr << '(' << line << ')';

            std::cerr << " : Error! "  << x.what() << std::endl;
            throw compilation_error();
        }

        return function();
    }

    void compile_all(
        utree const& ast,
        environment& env,
        actor_list& results,
        actor_list& fragments,
        std::string const& source_file = "")
    {
        int line = (ast.which() == utree_type::list_type)
            ? ast.tag() : 1;
        BOOST_FOREACH(utree const& program, ast)
        {
            scheme::function f;
            try
            {
                f = compile(program, env, fragments, line, source_file);
            }
            catch (compilation_error const& x)
            {
                continue; // try the next expression
            }
            results.push_back(f);
        }
    }

    void build_basic_environment(environment& env)
    {
        env.define("if", if_, 3);
        env.define("<", less_than, 2);
        env.define("<=", less_than_equal, 2);
        env.define("+", plus, -2);
        env.define("-", minus, -2);
        env.define("*", times, -2);
    }

    ///////////////////////////////////////////////////////////////////////////
    // interpreter
    ///////////////////////////////////////////////////////////////////////////
    struct interpreter : actor<interpreter>
    {
        template <typename Source>
        interpreter(
            Source& in,
            std::string const& source_file = "",
            environment* outer = 0)
        {
            if (outer == 0)
                build_basic_environment(env);

            if (input::parse_sexpr_list(in, program, source_file))
            {
                compile_all(program, env, flist, fragments, source_file);
            }
        }

        interpreter(utree const& program, environment* outer = 0)
        {
            if (outer == 0)
                build_basic_environment(env);
            compile_all(program, env, flist, fragments);
        }

        utree eval(args_type args) const
        {
            return flist.back()(args);
        }

        bool empty() const
        {
            return flist.empty() || flist.back().empty();
        }

        environment env;
        utree program;
        actor_list fragments;
        actor_list flist;
    };
}

#endif
