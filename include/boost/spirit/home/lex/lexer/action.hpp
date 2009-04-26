//  Copyright (c) 2001-2009 Hartmut Kaiser
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(SPIRIT_LEX_ACTION_NOV_18_2007_0743PM)
#define SPIRIT_LEX_ACTION_NOV_18_2007_0743PM

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once      // MS compatible compilers support #pragma once
#endif

#include <boost/spirit/home/lex/meta_compiler.hpp>
#include <boost/spirit/home/lex/lexer_type.hpp>
#include <boost/mpl/if.hpp>
#include <boost/type_traits/remove_const.hpp>
#include <boost/type_traits/is_same.hpp>

///////////////////////////////////////////////////////////////////////////////
namespace boost { namespace spirit { namespace lex
{
    ///////////////////////////////////////////////////////////////////////////
    template <typename Subject, typename Action>
    struct action : unary_lexer<action<Subject, Action> >
    {
        action(Subject const& subject, Action f)
          : subject(subject), f(f) {}

        template <typename LexerDef, typename String>
        void collect(LexerDef& lexdef, String const& state) const
        {
            // first collect the token definition information for the token_def 
            // this action is attached to
            subject.collect(lexdef, state);

            // retrieve the id of the associated token_def and register the 
            // given semantic action with the lexer instance
            lexdef.add_action(subject.id(), f);
        }

        Subject subject;
        Action f;
    };

}}}

///////////////////////////////////////////////////////////////////////////////
namespace boost { namespace spirit
{
    ///////////////////////////////////////////////////////////////////////////
    // Karma action meta-compiler
    template <>
    struct make_component<lex::domain, tag::action>
    {
        template <typename Sig>
        struct result;

        template <typename This, typename Elements, typename Modifiers>
        struct result<This(Elements, Modifiers)>
        {
            typedef typename
                remove_const<typename Elements::car_type>::type
            subject_type;

            typedef typename
                remove_const<typename Elements::cdr_type::car_type>::type
            action_type;

            typedef lex::action<subject_type, action_type> type;
        };

        template <typename Elements>
        typename result<make_component(Elements, unused_type)>::type
        operator()(Elements const& elements, unused_type) const
        {
            typename result<make_component(Elements, unused_type)>::type
                result(elements.car, elements.cdr.car);
            return result;
        }
    };
}}

#endif
