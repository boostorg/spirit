/*=============================================================================
    Copyright (c) 2001-2010 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_SCHEME_INTINSICS)
#define BOOST_SPIRIT_SCHEME_INTINSICS

#include "scheme_interpreter.hpp"

namespace scheme
{
    ///////////////////////////////////////////////////////////////////////////
    // if
    ///////////////////////////////////////////////////////////////////////////
    struct if_function
    {
        actor cond;
        actor then;
        actor else_;
        if_function(
            actor const& cond, actor const& then, actor const& else_)
          : cond(cond), then(then), else_(else_) {}

        typedef utree result_type;
        utree operator()(utree const& args) const
        {
            return cond(args).as<bool>() ? then(args) : else_(args);
        }
    };

    struct if_composer : composite<if_composer>
    {
        using base_type::operator();
        actor operator()(actor_list const& elements) const
        {
            actor_list::const_iterator i = elements.begin();
            actor if_ = *i++;
            actor then = *i++;
            actor else_ = *i;
            return actor(if_function(if_, then, else_));
        }
    };

    function_composer const if_
        = function_composer(if_composer());

    ///////////////////////////////////////////////////////////////////////////
    // less_than_equal
    ///////////////////////////////////////////////////////////////////////////
    struct less_than_equal_function
    {
        actor a;
        actor b;
        less_than_equal_function(actor const& a, actor const& b)
          : a(a), b(b) {}

        typedef utree result_type;
        utree operator()(utree const& args) const
        {
            return a(args) <= b(args);
        }
    };

    struct less_than_equal_composer : composite<less_than_equal_composer>
    {
        using base_type::operator();
        actor operator()(actor_list const& elements) const
        {
            actor_list::const_iterator i = elements.begin();
            actor a = *i++;
            actor b = *i;
            return actor(less_than_equal_function(a, b));
        }
    };

    function_composer const less_than_equal
        = function_composer(less_than_equal_composer());

    function_composer const lte = less_than_equal; // synonym

    ///////////////////////////////////////////////////////////////////////////
    // vararg_function
    ///////////////////////////////////////////////////////////////////////////
    template <typename Derived>
    struct vararg_function : composite<Derived>
    {
        typedef vararg_function<Derived> base_type;
        actor_list elements;
        vararg_function(actor_list const& elements)
          : elements(elements) {}

        using composite<Derived>::operator();
        utree operator()(utree const& args) const
        {
            actor_list::const_iterator i = elements.begin();
            utree result = (*i++)(args);
            boost::iterator_range<actor_list::const_iterator>
                rest(i++, elements.end());
            BOOST_FOREACH(actor const& element, rest)
            {
                derived().eval(result, element(args));
            }
            return result;
        }

        Derived const& derived() const
        {
            return *static_cast<Derived const*>(this);
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    // plus
    ///////////////////////////////////////////////////////////////////////////
    struct plus_function : vararg_function<plus_function>
    {
        plus_function(actor_list const& elements)
          : base_type(elements) {}

        void eval(utree& result, utree const& element) const
        {
            result = result + element;
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

    function_composer const plus
        = function_composer(plus_composer());

    ///////////////////////////////////////////////////////////////////////////
    // minus
    ///////////////////////////////////////////////////////////////////////////
    struct minus_function : vararg_function<minus_function>
    {
        minus_function(actor_list const& elements)
          : base_type(elements) {}

        void eval(utree& result, utree const& element) const
        {
            result = result - element;
        }
    };

    struct minus_composer
    {
        typedef actor result_type;
        actor operator()(actor_list const& elements) const
        {
            return actor(minus_function(elements));
        }
    };

    function_composer const minus
        = function_composer(minus_composer());

    ///////////////////////////////////////////////////////////////////////////
    // times
    ///////////////////////////////////////////////////////////////////////////
    struct times_function : vararg_function<times_function>
    {
        times_function(actor_list const& elements)
          : base_type(elements) {}

        void eval(utree& result, utree const& element) const
        {
            result = result * element;
        }
    };

    struct times_composer
    {
        typedef actor result_type;
        actor operator()(actor_list const& elements) const
        {
            return actor(times_function(elements));
        }
    };

    function_composer const times
        = function_composer(times_composer());
}

#endif
