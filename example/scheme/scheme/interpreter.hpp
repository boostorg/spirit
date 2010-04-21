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
#include <utree/utree.hpp>

#define SCHEME_COMPOSITE_LIMIT 10

namespace scheme
{
///////////////////////////////////////////////////////////////////////////////
//  The runtime interpreter
///////////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////
    // typedefs
    ///////////////////////////////////////////////////////////////////////////
    struct function;

    typedef std::list<function> actor_list;
    typedef boost::function<utree(args_type args)> stored_function;

    ///////////////////////////////////////////////////////////////////////////
    // actor
    ///////////////////////////////////////////////////////////////////////////
    template <typename Derived>
    struct actor
    {
        typedef utree result_type;
        typedef actor<Derived> base_type;

        utree operator()(args_type args) const
        {
            return derived().eval(args);
        }

        utree operator()() const
        {
            return derived().eval(args_type());
        }

        template <typename A0>
        utree operator()(A0 const& _0) const
        {
            boost::array<utree, 1> elements;
            elements[0] = _0;
            return derived().eval(get_range(elements));
        }

        template <typename A0, typename A1>
        utree operator()(A0 const& _0, A1 const& _1) const
        {
            boost::array<utree, 2> elements;
            elements[0] = _0;
            elements[1] = _1;
            return derived().eval(get_range(elements));
        }

        // More operators
        #include <scheme/detail/function_call.hpp>

        template <std::size_t n>
        static args_type
        get_range(boost::array<utree, n> const& array)
        {
            return args_type(array.begin(), array.end());
        }

        Derived const& derived() const
        {
            return *static_cast<Derived const*>(this);
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    // function
    ///////////////////////////////////////////////////////////////////////////
    struct function : actor<function>
    {
        stored_function f;
        function()
          : f() {}

        function(stored_function const& f)
          : f(f)
        {
            BOOST_ASSERT(!f.empty());
        }

        bool empty() const
        {
            return f.empty();
        }

        utree eval(args_type args) const
        {
            return f(args);
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    // values
    ///////////////////////////////////////////////////////////////////////////
    struct value_function : actor<value_function>
    {
        utree val;
        value_function(utree const& val) : val(val) {}

        utree eval(args_type /*args*/) const
        {
            return utree(boost::ref(val));
        }
    };

    struct value
    {
        typedef function result_type;
        function operator()(utree const& val) const
        {
            return function(value_function(val));
        }
    };

    value const val = {};

    ///////////////////////////////////////////////////////////////////////////
    // arguments
    ///////////////////////////////////////////////////////////////////////////
    struct argument_function : actor<argument_function>
    {
        std::size_t n;
        argument_function(std::size_t n) : n(n) {}

        utree eval(args_type args) const
        {
            return utree(boost::ref(args[n]));
        }
    };

    struct argument
    {
        typedef function result_type;
        function operator()(std::size_t n) const
        {
            return function(argument_function(n));
        }
    };

    argument const arg = {};
    function const _1 = arg(0);
    function const _2 = arg(1);
    function const _3 = arg(2);
    function const _4 = arg(3);
    function const _5 = arg(4);
    function const _6 = arg(5);
    function const _7 = arg(6);
    function const _8 = arg(7);
    function const _9 = arg(8);
    function const _10 = arg(10);

    ///////////////////////////////////////////////////////////////////////////
    // variable arguments.
    // Collects the arguments from n to last in a utree list.
    ///////////////////////////////////////////////////////////////////////////
    struct vararg_function : actor<vararg_function>
    {
        std::size_t n;
        vararg_function(std::size_t n) : n(n) {}

        utree eval(args_type args) const
        {
            utree result;
            for (std::size_t i = n; i < args.size(); ++i)
                result.push_back(boost::ref(args[i]));
            return result;
        }
    };

    struct vararg
    {
        typedef function result_type;
        function operator()(std::size_t n) const
        {
            return function(vararg_function(n));
        }
    };

    vararg const varg = {};
    function const _1_ = varg(0);
    function const _2_ = varg(1);
    function const _3_ = varg(2);
    function const _4_ = varg(3);
    function const _5_ = varg(4);
    function const _6_ = varg(5);
    function const _7_ = varg(6);
    function const _8_ = varg(7);
    function const _9_ = varg(8);
    function const _10_ = varg(10);

    ///////////////////////////////////////////////////////////////////////////
    // composite
    ///////////////////////////////////////////////////////////////////////////
    template <typename Derived>
    struct composite
    {
        typedef function result_type;
        typedef composite<Derived> base_type;

        function operator()(actor_list const& elements) const
        {
            return derived().compose(elements);
        }

        template <typename A0>
        function operator()(A0 const& _0) const
        {
            actor_list elements;
            elements.push_back(as_function(_0));
            return derived().compose(elements);
        }

        template <typename A0, typename A1>
        function operator()(A0 const& _0, A1 const& _1) const
        {
            actor_list elements;
            elements.push_back(as_function(_0));
            elements.push_back(as_function(_1));
            return derived().compose(elements);
        }

        // More operators
        #include <scheme/detail/composite_call.hpp>

        Derived const& derived() const
        {
            return *static_cast<Derived const*>(this);
        }

        template <typename T>
        static function as_function(T const& val)
        {
            return scheme::val(utree(val));
        }

        static function const& as_function(function const& f)
        {
            return f;
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    // unary_function
    ///////////////////////////////////////////////////////////////////////////
    template <typename Derived>
    struct unary_function : actor<unary_function<Derived> >
    {
        function a;
        typedef unary_function<Derived> base_type;

        unary_function(function const& a)
          : a(a)
        {
            BOOST_ASSERT(!a.empty());
        }

        utree eval(args_type args) const
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
        function compose(actor_list const& elements) const
        {
            return function(Function(elements.front()));
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    // binary_function
    ///////////////////////////////////////////////////////////////////////////
    template <typename Derived>
    struct binary_function : actor<binary_function<Derived> >
    {
        function a;
        function b;
        typedef binary_function<Derived> base_type;

        binary_function(function const& a, function const& b)
          : a(a), b(b)
        {
            BOOST_ASSERT(!a.empty());
            BOOST_ASSERT(!b.empty());
        }

        utree eval(args_type args) const
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
        function compose(actor_list const& elements) const
        {
            actor_list::const_iterator i = elements.begin();
            function a = *i++;
            function b = *i;
            return function(Function(a, b));
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    // nary_function
    ///////////////////////////////////////////////////////////////////////////
    template <typename Derived>
    struct nary_function : actor<nary_function<Derived> >
    {
        actor_list elements;
        typedef nary_function<Derived> base_type;

        nary_function(actor_list const& elements)
          : elements(elements)
        {
            BOOST_FOREACH(function const& element, elements)
            {
                BOOST_ASSERT(!element.empty());
            }
        }

        utree eval(args_type args) const
        {
            actor_list::const_iterator i = elements.begin();
            utree result = (*i++)(args);
            boost::iterator_range<actor_list::const_iterator>
                rest(i++, elements.end());
            BOOST_FOREACH(function const& element, rest)
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
        function compose(actor_list const& elements) const
        {
            return function(Function(elements));
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    // lambda
    ///////////////////////////////////////////////////////////////////////////
    struct lambda_function : actor<lambda_function>
    {
        actor_list elements;
        // we must hold f by reference because functions can be recursive
        boost::reference_wrapper<function const> f;

        lambda_function(function const& f, actor_list const& elements)
          : elements(elements), f(f) {}

        typedef utree result_type;
        utree eval(args_type args) const
        {
            if (!elements.empty())
            {
                boost::scoped_array<utree>
                    fargs(new utree[elements.size()]);
                std::size_t i = 0;
                BOOST_FOREACH(function const& element, elements)
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

    struct lambda : composite<lambda>
    {
        function f;

        lambda() : f() {}
        lambda(function const& f) : f(f) {}

        function compose(actor_list const& elements) const
        {
            return function(lambda_function(f, elements));
        }

        lambda& operator=(lambda const& other)
        {
            f = other.f;
            return *this;
        }

        lambda& operator=(function const& f_)
        {
            f = f_;
            return *this;
        }
    };
}

#endif
