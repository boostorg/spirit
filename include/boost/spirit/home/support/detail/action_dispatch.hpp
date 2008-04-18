/*=============================================================================
    Copyright (c) 2001-2008 Joel de Guzman
    Copyright (c) 2001-2008 Hartmut Kaiser
    http://spirit.sourceforge.net/

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_ACTION_DISPATCH_APR_18_2008_0720AM)
#define BOOST_SPIRIT_ACTION_DISPATCH_APR_18_2008_0720AM

#include <boost/spirit/home/support/detail/values.hpp>
#include <boost/spirit/home/phoenix/core/actor.hpp>

namespace boost { namespace spirit { namespace detail
{
    template <typename F, typename Attribute, typename Context>
    bool action_dispatch(F const& f, Attribute& attr, Context& context)
    {
        bool pass = true;
        f(attr, context, pass);
        return pass;
    }

    template <typename Eval, typename Attribute, typename Context>
    bool action_dispatch(phoenix::actor<Eval> const& f
      , Attribute& attr, Context& context)
    {
        bool pass = true;
        f(pass_value<Attribute>::call(attr), context, pass);
        return pass;
    }

    template <typename RT, typename A0, typename A1, typename A2
      , typename Attribute, typename Context>
    bool action_dispatch(RT(*f)(A0, A1, A2)
      , Attribute& attr, Context& context)
    {
        bool pass = true;
        f(attr, context, pass);
        return pass;
    }

    template <typename RT, typename A0, typename A1, typename A2
      , typename Attribute, typename Context>
    bool action_dispatch(RT(&f)(A0, A1, A2)
      , Attribute& attr, Context& context)
    {
        bool pass = true;
        f(attr, context, pass);
        return pass;
    }

    template <typename RT, typename A0, typename A1
      , typename Attribute, typename Context>
    bool action_dispatch(RT(*f)(A0, A1)
      , Attribute& attr, Context& context)
    {
        f(attr, context);
        return true;
    }

    template <typename RT, typename A0, typename A1
      , typename Attribute, typename Context>
    bool action_dispatch(RT(&f)(A0, A1)
      , Attribute& attr, Context& context)
    {
        f(attr, context);
        return true;
    }

    template <typename RT, typename A0
      , typename Attribute, typename Context>
    bool action_dispatch(RT(*f)(A0)
      , Attribute& attr, Context& context)
    {
        f(attr);
        return true;
    }

    template <typename RT, typename A0
      , typename Attribute, typename Context>
    bool action_dispatch(RT(&f)(A0)
      , Attribute& attr, Context& context)
    {
        f(attr);
        return true;
    }

}}}

#endif
