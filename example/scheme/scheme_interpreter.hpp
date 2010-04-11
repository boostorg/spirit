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
#include <boost/preprocessor/repetition/enum_params.hpp>

#define SCHEME_COMPOSITE_LIMIT 10

namespace scheme
{
///////////////////////////////////////////////////////////////////////////////
//  The runtime interpreter
///////////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////
    // actor
    ///////////////////////////////////////////////////////////////////////////
    struct actor
    {
        typedef utree result_type;

        actor()
          : f() {}

        actor(boost::function<utree(utree const& args)> const& f)
          : f(f) {}

        bool empty() const
        {
            return f.empty();
        }

        utree operator()() const
        {
            return f(utree());
        }

        utree operator()(utree const& x) const
        {
            if (x.which() == utree_type::list_type)
            {
                return f(x);
            }
            else
            {
                utree elements;
                elements.push_back(x);
                return f(elements);
            }
        }

        template <typename A0>
        utree operator()(A0 const& _0) const
        {
            utree elements;
            elements.push_back(_0);
            return f(elements);
        }

        template <typename A0, typename A1>
        utree operator()(A0 const& _0, A1 const& _1) const
        {
            utree elements;
            elements.push_back(_0);
            elements.push_back(_1);
            return f(elements);
        }

        // More operators
        #include "detail/scheme_function_call.hpp"

        boost::function<utree(utree const& args)> f;
    };

    typedef std::list<actor> actor_list;

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
        typedef actor result_type;
        actor operator()(utree const& val) const
        {
            return actor(value(val));
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
        typedef actor result_type;
        actor operator()(int n) const
        {
            return actor(argument(n));
        }
    };

    argument_composer const arg = {};
    actor const _1 = arg(0);
    actor const _2 = arg(1);
    actor const _3 = arg(2);
    actor const _4 = arg(3);
    actor const _5 = arg(4);
    actor const _6 = arg(5);
    actor const _7 = arg(6);
    actor const _8 = arg(7);
    actor const _9 = arg(8);
    actor const _10 = arg(10);

    ///////////////////////////////////////////////////////////////////////////
    // function_composer
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    inline actor as_function(T const& val)
    {
        return scheme::val(utree(val));
    }

    inline actor const& as_function(actor const& f)
    {
        return f;
    }

    struct function_composer
    {
        boost::function<actor(actor_list const&)> f;

        function_composer()
          : f(f) {}

        function_composer(boost::function<actor(actor_list const&)> const& f)
          : f(f) {}

        bool empty() const
        {
            return f.empty();
        }

        typedef actor result_type;

        actor operator()(actor_list const& elements) const
        {
            return f(elements);
        }

        template <typename A0>
        actor operator()(A0 const& _0) const
        {
            actor_list elements;
            elements.push_back(as_function(_0));
            return f(elements);
        }

        template <typename A0, typename A1>
        actor operator()(A0 const& _0, A1 const& _1) const
        {
            actor_list elements;
            elements.push_back(as_function(_0));
            elements.push_back(as_function(_1));
            return f(elements);
        }

        // More operators
        #include "detail/scheme_function_composer_call.hpp"
    };

    ///////////////////////////////////////////////////////////////////////////
    // plus
    ///////////////////////////////////////////////////////////////////////////
    struct plus_function
    {
        actor_list elements;
        plus_function(actor_list const& elements)
          : elements(elements) {}

        typedef utree result_type;
        utree operator()(utree const& args) const
        {
            utree result(0);
            BOOST_FOREACH(actor const& element, elements)
            {
                result = result + element(args);
            }
            return result;
        }
    };

    struct plus_composer
    {
        typedef actor result_type;
        actor operator()(actor_list const& elements) const
        {
            return actor(plus_function(elements));
        }
    };

    function_composer const plus = plus_composer();

    ///////////////////////////////////////////////////////////////////////////
    // call
    ///////////////////////////////////////////////////////////////////////////
    struct call_function
    {
        actor_list elements;
        actor f;
        call_function(actor const& f, actor_list const& elements)
          : elements(elements), f(f) {}

        typedef utree result_type;
        utree operator()(utree const& args) const
        {
            utree fargs;
            BOOST_FOREACH(actor const& element, elements)
            {
                fargs.push_back(element(args));
            }
            return f(fargs);
        }
    };

    struct call_composer
    {
        actor f;
        int arity;
        call_composer(actor const& f, int arity)
          : f(f), arity(arity) {}

        typedef actor result_type;
        actor operator()(actor_list const& elements) const
        {
            // $$$ use throw $$$
            BOOST_ASSERT(elements.size() == arity);
            return actor(call_function(f, elements));
        }
    };

    inline function_composer const call(actor const& f, int arity)
    {
        return call_composer(f, arity);
    }
}

#endif
