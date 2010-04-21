/*=============================================================================
    Copyright (c) 2001-2010 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_SCHEME_INTRINSICS)
#define BOOST_SPIRIT_SCHEME_INTRINSICS

#include <scheme/interpreter.hpp>
#include <utree/operators.hpp>

namespace scheme
{
    ///////////////////////////////////////////////////////////////////////////
    // if
    ///////////////////////////////////////////////////////////////////////////
    struct if_function : actor<if_function>
    {
        function cond;
        function then;
        function else_;
        if_function(
            function const& cond, function const& then, function const& else_)
          : cond(cond), then(then), else_(else_)
        {
            BOOST_ASSERT(!cond.empty());
            BOOST_ASSERT(!then.empty());
            BOOST_ASSERT(!else_.empty());
        }

        typedef utree result_type;
        utree eval(args_type args) const
        {
            return cond(args).get<bool>() ? then(args) : else_(args);
        }
    };

    struct if_composite : composite<if_composite>
    {
        function compose(actor_list const& elements) const
        {
            actor_list::const_iterator i = elements.begin();
            function if_ = *i++;
            function then = *i++;
            function else_ = *i;
            return function(if_function(if_, then, else_));
        }
    };

    if_composite const if_ = if_composite();

    ///////////////////////////////////////////////////////////////////////////
    // less_than
    ///////////////////////////////////////////////////////////////////////////
    struct less_than_function
      : binary_function<less_than_function>
    {
        less_than_function(function const& a, function const& b)
          : base_type(a, b) {}

        typedef utree result_type;
        utree eval(utree const& a, utree const& b) const
        {
            return a < b;
        }
    };

    struct less_than_composite
      : binary_composite<less_than_function> {};

    less_than_composite const less_than
        = less_than_composite();
    less_than_composite const lt = less_than; // synonym

    ///////////////////////////////////////////////////////////////////////////
    // less_than_equal
    ///////////////////////////////////////////////////////////////////////////
    struct less_than_equal_function
      : binary_function<less_than_equal_function>
    {
        less_than_equal_function(function const& a, function const& b)
          : base_type(a, b) {}

        typedef utree result_type;
        utree eval(utree const& a, utree const& b) const
        {
            return a <= b;
        }
    };

    struct less_than_equal_composite
      : binary_composite<less_than_equal_function> {};

    less_than_equal_composite const less_than_equal
        = less_than_equal_composite();
    less_than_equal_composite const lte = less_than_equal; // synonym

    ///////////////////////////////////////////////////////////////////////////
    // plus
    ///////////////////////////////////////////////////////////////////////////
    struct plus_function : nary_function<plus_function>
    {
        plus_function(actor_list const& elements)
          : base_type(elements) {}

        bool eval(utree& result, utree const& element) const
        {
            result = result + element;
            return true;
        }
    };

    struct plus_composite : nary_composite<plus_function> {};
    plus_composite const plus = plus_composite();

    ///////////////////////////////////////////////////////////////////////////
    // minus
    ///////////////////////////////////////////////////////////////////////////
    struct minus_function : nary_function<minus_function>
    {
        minus_function(actor_list const& elements)
          : base_type(elements) {}

        bool eval(utree& result, utree const& element) const
        {
            result = result - element;
            return true;
        }
    };

    struct minus_composite : nary_composite<minus_function> {};
    minus_composite const minus = minus_composite();

    ///////////////////////////////////////////////////////////////////////////
    // times
    ///////////////////////////////////////////////////////////////////////////
    struct times_function : nary_function<times_function>
    {
        times_function(actor_list const& elements)
          : base_type(elements) {}

        bool eval(utree& result, utree const& element) const
        {
            result = result * element;
            return true;
        }
    };

    struct times_composite : nary_composite<times_function> {};
    times_composite const times = times_composite();
}

#endif
