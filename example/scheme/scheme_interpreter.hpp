/*=============================================================================
    Copyright (c) 2001-2010 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_SCHEME_INTERPRETER)
#define BOOST_SPIRIT_SCHEME_INTERPRETER

#include <list>
#include <boost/function.hpp>
#include <boost/foreach.hpp>
#include <boost/array.hpp>
#include <boost/scoped_array.hpp>
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
    struct actor;

    typedef std::list<actor> actor_list;
    typedef boost::iterator_range<utree const*> args_type;
    typedef boost::function<utree(args_type args)> actor_function;

    struct actor
    {
        typedef utree result_type;

        actor()
          : f() {}

        actor(actor_function const& f)
          : f(f)
        {
            BOOST_ASSERT(!f.empty());
        }

        bool empty() const
        {
            return f.empty();
        }

        utree operator()() const
        {
            return f(args_type());
        }

        utree operator()(args_type x) const
        {
            return f(x);
        }

        template <typename A0>
        utree operator()(A0 const& _0) const
        {
            boost::array<utree, 1> elements;
            elements[0] = _0;
            return f(get_range(elements));
        }

        template <typename A0, typename A1>
        utree operator()(A0 const& _0, A1 const& _1) const
        {
            boost::array<utree, 2> elements;
            elements[0] = _0;
            elements[1] = _1;
            return f(get_range(elements));
        }

        // More operators
        #include "detail/scheme_function_call.hpp"

        actor_function f;

        template <std::size_t n>
        static args_type
        get_range(boost::array<utree, n> const& array)
        {
            return args_type(array.begin(), array.end());
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    // values
    ///////////////////////////////////////////////////////////////////////////
    struct value_function
    {
        utree val;
        value_function(utree const& val) : val(val) {}

        typedef utree result_type;
        utree operator()(args_type /*args*/) const
        {
            return utree(boost::ref(val));
        }
    };

    struct value
    {
        typedef actor result_type;
        actor operator()(utree const& val) const
        {
            return actor(value_function(val));
        }
    };

    value const val = {};

    ///////////////////////////////////////////////////////////////////////////
    // arguments
    ///////////////////////////////////////////////////////////////////////////
    struct argument_function
    {
        std::size_t n;
        argument_function(std::size_t n) : n(n) {}

        typedef utree result_type;
        utree operator()(args_type args) const
        {
            return utree(boost::ref(*(args.begin()+n)));
        }
    };

    struct argument
    {
        typedef actor result_type;
        actor operator()(std::size_t n) const
        {
            return actor(argument_function(n));
        }
    };

    argument const arg = {};
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
    // composite
    ///////////////////////////////////////////////////////////////////////////
    template <typename Derived>
    struct composite
    {
        typedef actor result_type;
        typedef composite<Derived> base_type;

        actor operator()(actor_list const& elements) const
        {
            return derived()(elements);
        }

        template <typename A0>
        actor operator()(A0 const& _0) const
        {
            actor_list elements;
            elements.push_back(as_function(_0));
            return derived()(elements);
        }

        template <typename A0, typename A1>
        actor operator()(A0 const& _0, A1 const& _1) const
        {
            actor_list elements;
            elements.push_back(as_function(_0));
            elements.push_back(as_function(_1));
            return derived()(elements);
        }

        // More operators
        #include "detail/scheme_composite_call.hpp"

        Derived const& derived() const
        {
            return *static_cast<Derived const*>(this);
        }

        template <typename T>
        static actor as_function(T const& val)
        {
            return scheme::val(utree(val));
        }

        static actor const& as_function(actor const& f)
        {
            return f;
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    // unary_function
    ///////////////////////////////////////////////////////////////////////////
    template <typename Derived>
    struct unary_function
    {
        actor a;
        typedef unary_function<Derived> base_type;

        unary_function(actor const& a)
          : a(a)
        {
            BOOST_ASSERT(!a.empty());
        }

        typedef utree result_type;
        utree operator()(args_type args) const
        {
            return derived().eval(a(args));
        }

        Derived const& derived() const
        {
            return *static_cast<Derived const*>(this);
        }
    };

    template <typename Function>
    struct unary_composite : composite<unary_composite<Function> >
    {
        using composite<unary_composite<Function> >::operator();
        actor operator()(actor_list const& elements) const
        {
            return actor(Function(elements.front()));
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    // binary_function
    ///////////////////////////////////////////////////////////////////////////
    template <typename Derived>
    struct binary_function
    {
        actor a;
        actor b;
        typedef binary_function<Derived> base_type;

        binary_function(actor const& a, actor const& b)
          : a(a), b(b)
        {
            BOOST_ASSERT(!a.empty());
            BOOST_ASSERT(!b.empty());
        }

        typedef utree result_type;
        utree operator()(args_type args) const
        {
            return derived().eval(a(args), b(args));
        }

        Derived const& derived() const
        {
            return *static_cast<Derived const*>(this);
        }
    };

    template <typename Function>
    struct binary_composite : composite<binary_composite<Function> >
    {
        using composite<binary_composite<Function> >::operator();
        actor operator()(actor_list const& elements) const
        {
            actor_list::const_iterator i = elements.begin();
            actor a = *i++;
            actor b = *i;
            return actor(Function(a, b));
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    // nary_function
    ///////////////////////////////////////////////////////////////////////////
    template <typename Derived>
    struct nary_function : composite<Derived>
    {
        typedef nary_function<Derived> base_type;
        actor_list elements;
        nary_function(actor_list const& elements)
          : elements(elements)
        {
            BOOST_FOREACH(actor const& element, elements)
            {
                BOOST_ASSERT(!element.empty());
            }
        }

        using composite<Derived>::operator();
        utree operator()(args_type args) const
        {
            actor_list::const_iterator i = elements.begin();
            utree result = (*i++)(args);
            boost::iterator_range<actor_list::const_iterator>
                rest(i++, elements.end());
            BOOST_FOREACH(actor const& element, rest)
            {
                if (!derived().eval(result, element(args)))
                    break; // allow short-circuit evaluation
            }
            return result;
        }

        Derived const& derived() const
        {
            return *static_cast<Derived const*>(this);
        }
    };

    template <typename Function>
    struct nary_composite : composite<nary_composite<Function> >
    {
        using composite<nary_composite<Function> >::operator();
        actor operator()(actor_list const& elements) const
        {
            return actor(Function(elements));
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    // function
    ///////////////////////////////////////////////////////////////////////////
    struct lambda_function
    {
        actor_list elements;
        // we must hold f by reference because functions can be recursive
        boost::reference_wrapper<actor const> f;

        lambda_function(actor const& f, actor_list const& elements)
          : elements(elements), f(f) {}

        typedef utree result_type;
        utree operator()(args_type args) const
        {
            if (!elements.empty())
            {
                boost::scoped_array<utree>
                    fargs(new utree[elements.size()]);
                std::size_t i = 0;
                BOOST_FOREACH(actor const& element, elements)
                {
                    fargs[i++] = element(args);
                }
                utree const* fi = fargs.get();
                return f.get()(args_type(fi, fi+elements.size()));
            }
            else
            {
                return f.get()();
            }
        }
    };

    struct function : composite<function>
    {
        actor f;

        function() : f() {}
        function(actor const& f) : f(f) {}

        using base_type::operator();
        actor operator()(actor_list const& elements) const
        {
            return actor(lambda_function(f, elements));
        }

        function& operator=(function const& other)
        {
            f = other.f;
            return *this;
        }

        function& operator=(actor const& f_)
        {
            f = f_;
            return *this;
        }
    };
}

#endif
