/*=============================================================================
    Copyright (c) 2001-2010 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_SCHEME_INTERPRETER)
#define BOOST_SPIRIT_SCHEME_INTERPRETERnamespace scheme

#include <list>
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
    // plus
    ///////////////////////////////////////////////////////////////////////////
    struct plus_function
    {
        function_list elements;
        plus_function(function_list const& elements)
          : elements(elements) {}

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
        function operator()(function_list const& elements)
        {
            return function(plus_function(elements));
        }
    };

    plus_composer const plus = {};

    ///////////////////////////////////////////////////////////////////////////
    // lambda
    ///////////////////////////////////////////////////////////////////////////
    struct lambda_function
    {
        actor_list elements;
        actor f;
        lambda_function(actor const& f, actor_list const& elements)
          : elements(elements), f(f) {}

        typedef utree result_type;
        utree operator()(utree const& args) const
        {
            utree fargs;
            BOOST_FOREACH(function const& element, elements)
            {
                fargs.push_back(element(args));
            }
            return f(fargs);
        }
    };

    struct lambda_composer
    {
        function f;
        int arity;
        lambda_composer(actor const& f, int arity)
          : f(f), arity(arity) {}

        typedef function result_type;
        function operator()(function_list const& elements)
        {
            // $$$ use throw $$$
            BOOST_ASSERT(elements.size() == arity);
            return actor(lambda_function(f, elements));
        }
    };

    inline function_composer const lambda(actor const& f, int arity)
    {
        return lambda_composer(f, arity);
    }
}

#endif
