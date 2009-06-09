//  Copyright (c) 2001-2009 Hartmut Kaiser
//  Copyright (c) 2001-2009 Joel de Guzman
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(BOOST_SPIRIT_LEX_ARGUMENT_JUNE_07_2009_1106AM)
#define BOOST_SPIRIT_LEX_ARGUMENT_JUNE_07_2009_1106AM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/phoenix/core/actor.hpp>
#include <boost/spirit/home/phoenix/core/argument.hpp>
#include <boost/spirit/home/phoenix/core/compose.hpp>
#include <boost/spirit/home/phoenix/core/value.hpp>
#include <boost/spirit/home/phoenix/core/as_actor.hpp>
#include <boost/spirit/home/phoenix/operator/self.hpp>
#include <boost/spirit/home/support/string_traits.hpp>
#include <boost/fusion/include/at.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/type_traits/remove_const.hpp>
#include <boost/utility/enable_if.hpp>

///////////////////////////////////////////////////////////////////////////////
namespace boost { namespace spirit
{
    ///////////////////////////////////////////////////////////////////////////
    //  The state_getter is a Phoenix actor used to access the name of the 
    //  current lexer state by calling get_state_name() on the context (which 
    //  is the 4th parameter to any lexer semantic actions).
    //
    //  This Phoenix actor is invoked whenever the placeholder '_state' is used
    //  as a rvalue inside a lexer semantic action:
    //
    //      lex::token_def<> identifier = "[a-zA-Z_][a-zA-Z0-9_]*";
    //      this->self = identifier [ std::cout << _state ];
    //
    //  The example shows how to print the lexer state after matching a token
    //  'identifier'.
    struct state_getter
    {
        typedef mpl::true_ no_nullary;

        template <typename Env>
        struct result
        {
            typedef typename
                remove_const<
                    typename mpl::at_c<typename Env::args_type, 4>::type
                >::type
            context_type;

            typedef typename context_type::state_name_type type;
        };

        template <typename Env>
        typename result<Env>::type
        eval(Env const& env) const
        {
            return fusion::at_c<4>(env.args()).get_state_name();
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    //  The state_setter is a Phoenix actor used to change the name of the 
    //  current lexer state by calling set_state_name() on the context (which 
    //  is the 4th parameter to any lexer semantic actions).
    //
    //  This Phoenix actor is invoked whenever the placeholder '_state' is used
    //  as a lvalue inside a lexer semantic action:
    //
    //      lex::token_def<> identifier = "[a-zA-Z_][a-zA-Z0-9_]*";
    //      this->self = identifier [ _state = "SOME_LEXER_STATE" ];
    //
    //  The example shows how to change the lexer state after matching a token
    //  'identifier'.
    template <typename Actor>
    struct state_setter
    {
        typedef mpl::true_ no_nullary;

        template <typename Env>
        struct result
        {
            typedef void type;
        };

        template <typename Env>
        void eval(Env const& env) const
        {
            fusion::at_c<4>(env.args()).set_state_name(
                traits::get_c_string(actor_()));
        }

        state_setter(Actor const& actor)
          : actor_(actor) {}

        Actor actor_;
    };

    ///////////////////////////////////////////////////////////////////////////
    //  The state_context is used as a noop Phoenix actor to create the 
    //  placeholder '_state' (see below). It is a noop actor because it is used
    //  as a placeholder only, while it is being converted either to a 
    //  state_getter (if used as a rvalue) or to a state_setter (if used as a 
    //  lvalue). The conversion is achieved by specializing and overloading a 
    //  couple of the Phoenix templates and functions from the Phoenix 
    //  expression composition engine (see the end of this file).
    struct state_context 
    {
        typedef mpl::true_ no_nullary;

        template <typename Env>
        struct result
        {
            typedef unused_type type;
        };

        template <typename Env>
        unused_type
        eval(Env const& env) const
        {
            return unused;
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    //  The eoi_getter is used to create the _eoi placeholder, which is a 
    //  Phoenix actor used to access the end of input iterator pointing to the 
    //  end of the underlying input sequence.
    //
    //  This actor is invoked whenever the placeholder '_eoi' is used in a
    //  lexer semantic action:
    //
    //      lex::token_def<> identifier = "[a-zA-Z_][a-zA-Z0-9_]*";
    //      this->self = identifier 
    //          [ std::cout << construct_<std::string>(_end, _eoi) ];
    //
    //  The example shows how to use _eoi to print all remaining input after
    //  matching a token 'identifier'.
    struct eoi_getter
    {
        typedef mpl::true_ no_nullary;

        template <typename Env>
        struct result
        {
            typedef typename
                remove_const<
                    typename mpl::at_c<typename Env::args_type, 4>::type
                >::type
            context_type;

            typedef typename context_type::base_iterator_type const& type;
        };

        template <typename Env>
        typename result<Env>::type 
        eval(Env const& env) const
        {
            return fusion::at_c<4>(env.args()).get_eoi();
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    // '_start' and '_end' may be used to access the start and the end of 
    // the matched sequence of the current token
    phoenix::actor<phoenix::argument<0> > const _start = phoenix::argument<0>();
    phoenix::actor<phoenix::argument<1> > const _end = phoenix::argument<1>();

    // We are reusing the placeholder '_pass' to access and change the pass
    // status of the current match (see support/argument.hpp for its 
    // definition).

    // '_tokenid' may be used to access and change the tokenid of the current 
    // token
    phoenix::actor<phoenix::argument<3> > const _tokenid = phoenix::argument<3>();

    // _state may be used to access and change the name of the current lexer 
    // state
    phoenix::actor<state_context> const _state = state_context();

    // '_eoi' may be used to access the end of input iterator of the input 
    // stream used by the lexer to match tokens from
    phoenix::actor<eoi_getter> const _eoi = eoi_getter();

}}

///////////////////////////////////////////////////////////////////////////////
namespace boost { namespace phoenix
{
    ///////////////////////////////////////////////////////////////////////////
    //  The specialization of as_actor_base<> below is needed to convert all
    //  occurrences of _state in places where it's used as a rvalue into the 
    //  proper Phoenix actor (spirit::state_getter) accessing the lexer state.
    template<>
    struct as_actor_base<actor<spirit::state_context> >
    {
        typedef spirit::state_getter type;

        static spirit::state_getter
        convert(actor<spirit::state_context>)
        {
            return spirit::state_getter();
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    //  The specialization of as_composite<> below is needed to integrate the 
    //  overload of the compose() function into Phoenix expression composition 
    //  engine.
    template <typename RHS>
    struct as_composite<assign_eval, actor<spirit::state_context>, RHS>
    {
        typedef spirit::state_setter<typename as_actor<RHS>::type> type;
    };

    ///////////////////////////////////////////////////////////////////////////
    //  The compose function overload allows to convert all occurences of 
    //  _state in places where it's used as a lvalue into the proper Phoenix 
    //  actor (spirit::state_setter) allowing to change the lexer state.
    template <typename EvalPolicy, typename RHS>
    inline spirit::state_setter<typename as_actor<RHS>::type>
    compose(actor<spirit::state_context>, RHS const& rhs
      , typename enable_if<is_same<EvalPolicy, assign_eval> >::type* = NULL)
    {
        typedef typename as_actor<RHS>::type actor_type;
        return spirit::state_setter<actor_type>(as_actor<RHS>::convert(rhs));
    }

}}

#undef SPIRIT_DECLARE_ARG
#endif

