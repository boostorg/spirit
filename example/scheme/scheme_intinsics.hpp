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
        utree operator()(args_type args) const
        {
            return cond(args).as<bool>() ? then(args) : else_(args);
        }
    };

    struct if_composite : composite<if_composite>
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

    if_composite const if_ = if_composite();

    ///////////////////////////////////////////////////////////////////////////
    // less_than
    ///////////////////////////////////////////////////////////////////////////
    struct less_than_function
      : binary_function<less_than_function>
    {
        less_than_function(actor const& a, actor const& b)
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
        less_than_equal_function(actor const& a, actor const& b)
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

    struct plus_composite : vararg_composite<plus_function> {};
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

    struct minus_composite : vararg_composite<minus_function> {};
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

    struct times_composite : vararg_composite<times_function> {};
    times_composite const times = times_composite();
}

#endif
