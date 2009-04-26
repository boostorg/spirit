/*=============================================================================
    Copyright (c) 2001-2009 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_SPIRIT_LAZY_MARCH_27_2007_1002AM)
#define BOOST_SPIRIT_LAZY_MARCH_27_2007_1002AM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/qi/domain.hpp>
#include <boost/spirit/home/qi/skip_over.hpp>
#include <boost/spirit/home/qi/meta_compiler.hpp>
#include <boost/spirit/home/support/attributes.hpp>
#include <boost/spirit/home/support/unused.hpp>
#include <boost/spirit/home/support/info.hpp>
#include <boost/spirit/home/support/lazy.hpp>
#include <boost/spirit/home/phoenix/core/actor.hpp>
#include <boost/fusion/include/at.hpp>
#include <boost/utility/result_of.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/mpl/not.hpp>

namespace boost { namespace spirit
{
    ///////////////////////////////////////////////////////////////////////////
    // Enablers
    ///////////////////////////////////////////////////////////////////////////
    template <typename Eval>
    struct use_terminal<qi::domain, phoenix::actor<Eval> >  // enables phoenix actors
        : mpl::true_ {};

    // forward declaration
    template <typename Terminal, typename Actor, int Arity>
    struct lazy_terminal;
}}

namespace boost { namespace spirit { namespace qi
{
    using spirit::lazy;

    template <typename Function>
    struct lazy_parser : parser<lazy_parser<Function> >
    {
        template <typename Context, typename Iterator>
        struct attribute
        {
            typedef typename
                remove_reference<
                    typename boost::result_of<Function(unused_type, Context)>::type
                >::type
            expr_type;

            // If you got an error_invalid_expression error message here,
            // then the expression (expr_type) is not a valid spirit qi
            // expression.
            BOOST_SPIRIT_ASSERT_MATCH(qi::domain, expr_type)

            typedef typename
                result_of::compile<qi::domain, expr_type, tag::lazy_eval>::type
            parser_type;

            typedef typename
                traits::attribute_of<parser_type, Context, Iterator>::type
            type;
        };

        lazy_parser(Function const& function)
          : function(function) {}

        template <typename Iterator, typename Context
          , typename Skipper, typename Attribute>
        bool parse(Iterator& first, Iterator const& last
          , Context& context, Skipper const& skipper
          , Attribute& attr) const
        {
            return compile<qi::domain>(function(unused, context), tag::lazy_eval())
                .parse(first, last, context, skipper, attr);
        }

        template <typename Context>
        info what(Context& context) const
        {
            return info("lazy"
              , compile<qi::domain>(function(unused, context), tag::lazy_eval())
                    .what(context)
            );
        }

        Function function;
    };


    template <typename Function, typename Subject>
    struct lazy_directive : unary_parser<lazy_directive<Function, Subject> >
    {
        typedef Subject subject_type;

        template <typename Context, typename Iterator>
        struct attribute
        {
            typedef typename
                remove_reference<
                    typename boost::result_of<Function(unused_type, Context)>::type
                >::type
            directive_expr_type;

            typedef typename
                proto::result_of::make_expr<
                    proto::tag::subscript
                  , directive_expr_type
                  , Subject
                >::type
            expr_type;

            // If you got an error_invalid_expression error message here,
            // then the expression (expr_type) is not a valid spirit qi
            // expression.
            BOOST_SPIRIT_ASSERT_MATCH(qi::domain, expr_type)

            typedef typename
                result_of::compile<qi::domain, expr_type, tag::lazy_eval>::type
            parser_type;

            typedef typename
                traits::attribute_of<parser_type, Context, Iterator>::type
            type;
        };

        lazy_directive(Function const& function, Subject const& subject)
          : function(function), subject(subject) {}

        template <typename Iterator, typename Context
          , typename Skipper, typename Attribute>
        bool parse(Iterator& first, Iterator const& last
          , Context& context, Skipper const& skipper
          , Attribute& attr) const
        {
            return compile<qi::domain>(
                proto::make_expr<proto::tag::subscript>(
                    function(unused, context)
                  , subject
                ), tag::lazy_eval())
                .parse(first, last, context, skipper, attr);
        }

        template <typename Context>
        info what(Context& context) const
        {
            return info("lazy-directive"
              , compile<qi::domain>(
                    proto::make_expr<proto::tag::subscript>(
                        function(unused, context)
                      , subject
                    ), tag::lazy_eval())
                    .what(context)
            );
        }

        Function function;
        Subject subject;
    };

    ///////////////////////////////////////////////////////////////////////////
    // Parser generators: make_xxx function (objects)
    ///////////////////////////////////////////////////////////////////////////
    template <typename Eval, typename Modifiers>
    struct make_primitive<phoenix::actor<Eval>, Modifiers>
    {
        typedef lazy_parser<phoenix::actor<Eval> > result_type;
        result_type operator()(phoenix::actor<Eval> const& f, unused_type) const
        {
            return result_type(f);
        }
    };

    template <typename Terminal, typename Actor, int Arity, typename Modifiers>
    struct make_primitive<lazy_terminal<Terminal, Actor, Arity>, Modifiers>
    {
        typedef lazy_parser<Actor> result_type;
        result_type operator()(
            lazy_terminal<Terminal, Actor, Arity> const& lt, unused_type) const
        {
            return result_type(lt.actor);
        }
    };

    template <typename Terminal, typename Actor, int Arity, typename Subject, typename Modifiers>
    struct make_directive<lazy_terminal<Terminal, Actor, Arity>, Subject, Modifiers>
    {
        typedef lazy_directive<Actor, Subject> result_type;
        result_type operator()(
            lazy_terminal<Terminal, Actor, Arity> const& lt
          , Subject const& subject, unused_type) const
        {
            return result_type(lt.actor, subject);
        }
    };
}}}

#endif
