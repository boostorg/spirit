/*=============================================================================
    Copyright (c) 2001-2008 Joel de Guzman
    Copyright (c) 2001-2008 Hartmut Kaiser
    http://spirit.sourceforge.net/

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_WRAP_ACTION_APR_19_2008_0103PM)
#define BOOST_SPIRIT_WRAP_ACTION_APR_19_2008_0103PM

#include <boost/spirit/home/support/detail/values.hpp>
#include <boost/spirit/home/phoenix/core/actor.hpp>
#include <boost/spirit/home/phoenix/core/argument.hpp>
#include <boost/spirit/home/phoenix/bind.hpp>

#include <boost/bind.hpp>

///////////////////////////////////////////////////////////////////////////////
namespace boost { namespace spirit { namespace lex { namespace detail
{
    template <typename FunctionType, typename Attribute, typename Context>
    struct wrap_action
    {
        // plain functors are not touched at all
        template <typename F>
        static FunctionType call(F const& f)
        {
            return f;
        }

        // wrap phoenix actor, make sure first argument is a fusion sequence
        template <typename Eval>
        static void phoenix_action(phoenix::actor<Eval> const& f,
            Attribute const& attr, std::size_t id, bool& pass, Context& ctx)
        {
            f(spirit::detail::pass_value<Attribute const>::call(attr), id, pass, ctx);
        }
        
//         struct phoenix_action
//         {
//             template <typename F, typename T1, typename T2, typename T3, typename T4>
//             struct result { typedef void type; };
//             
//             template <typename Eval>
//             void 
//             operator()(phoenix::actor<Eval> const& f, Attribute const& attr, 
//                 std::size_t id, bool& pass, Context& ctx) const
//             {
//                 f(spirit::detail::pass_value<Attribute const>::call(attr), id, pass, ctx);
//             }
//         };

        template <typename Eval>
        static FunctionType call(phoenix::actor<Eval> const& f)
        {
//             using phoenix::arg_names::arg1;
//             using phoenix::arg_names::arg2;
//             using phoenix::arg_names::arg3;
//             using phoenix::arg_names::arg4;

//             void (*pf)(phoenix::actor<Eval> const&,
//                 Attribute const&, std::size_t, bool&, Context&) =
//                     &wrap_action::phoenix_action;
// 
//             return phoenix::bind(pf, f, arg1, arg2, arg3, arg4);

//             return phoenix::bind(phoenix_action(), f, arg1, arg2, arg3, arg4);
            
            return boost::bind(
                &wrap_action::phoenix_action<Eval>, f, _1, _2, _3, _4);
        }

        // semantic actions with 3 arguments
        template <typename F>
        static void arg3_action(F* f, Attribute const& attr,
            std::size_t id, bool& pass, Context&)
        {
            f(attr, id, pass);
        }

        template <typename A0, typename A1, typename A2>
        static FunctionType call(void(*f)(A0, A1, A2))
        {
            using phoenix::arg_names::arg1;
            using phoenix::arg_names::arg2;
            using phoenix::arg_names::arg3;

            return phoenix::bind(
                &wrap_action::arg3_action<void(A0, A1, A2)>, f, arg1, arg2, arg3);
        }

        // semantic actions with 2 arguments
        template <typename F>
        static void arg2_action(F* f, Attribute const& attr,
            std::size_t id, bool&, Context&)
        {
            f(attr, id);
        }

        template <typename A0, typename A1>
        static FunctionType call(void(*f)(A0, A1))
        {
            using phoenix::arg_names::arg1;
            using phoenix::arg_names::arg2;

            return phoenix::bind(
                &wrap_action::arg2_action<void(A0, A1)>, f, arg1, arg2);
        }

        // semantic actions with 1 argument
        template <typename F>
        static void arg1_action(F* f, Attribute const& attr,
            std::size_t, bool&, Context&)
        {
            f(attr);
        }

        template <typename A0>
        static FunctionType call(void(*f)(A0))
        {
            using phoenix::arg_names::arg1;
            return phoenix::bind(&wrap_action::arg1_action<void(A0)>, f, arg1);
        }
    };

    // specialization allowing to skip wrapping for lexer types not supporting
    // semantic actions
    template <typename Attribute, typename Context>
    struct wrap_action<unused_type, Attribute, Context>
    {
        // plain functors are not touched at all
        template <typename F>
        static F const& call(F const& f)
        {
            return f;
        }
    };

}}}}

#endif
