//  Copyright (c) 2001-2009 Hartmut Kaiser
//  Copyright (c) 2001-2009 Joel de Guzman
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(BOOST_SPIRIT_KARMA_LAZY_MARCH_27_2007_1231PM)
#define BOOST_SPIRIT_KARMA_LAZY_MARCH_27_2007_1231PM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/karma/domain.hpp>
#include <boost/spirit/home/karma/delimit_out.hpp>
#include <boost/spirit/home/karma/meta_compiler.hpp>
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
    struct use_terminal<karma::domain, phoenix::actor<Eval> >  // enables phoenix actors
        : mpl::true_ {};

    // forward declaration
    template <typename Terminal, typename Actor, int Arity>
    struct lazy_terminal;

}}

namespace boost { namespace spirit { namespace karma
{
    using spirit::lazy;

    template <typename Function>
    struct lazy_generator : generator<lazy_generator<Function> >
    {
        template <typename Context, typename Unused>
        struct attribute
        {
            typedef typename
                remove_reference<
                    typename boost::result_of<Function(unused_type, Context)>::type
                >::type
            expr_type;

            // If you got an error_invalid_expression error message here,
            // then the expression (expr_type) is not a valid spirit karma
            // expression.
            BOOST_SPIRIT_ASSERT_MATCH(karma::domain, expr_type)

            typedef typename
                result_of::compile<karma::domain, expr_type, tag::lazy_eval>::type
            generator_type;

            typedef typename
                traits::attribute_of<generator_type, Context, Unused>::type
            type;
        };

        lazy_generator(Function const& func)
          : func(func) 
        {}

        template <
            typename OutputIterator, typename Context, 
            typename Delimiter, typename Attribute
        >
        bool generate(OutputIterator& sink, Context& context, 
            Delimiter const& d, Attribute const& attr) const
        {
            return compile<karma::domain>(func(unused, context), tag::lazy_eval())
                .generate(sink, context, d, attr);
        }

        template <typename Context>
        info what(Context& context) const
        {
            return info("lazy"
              , compile<karma::domain>(func(unused, context), tag::lazy_eval())
                    .what(context)
            );
        }

        Function func;
    };

    ///////////////////////////////////////////////////////////////////////////
    template <typename Function, typename Subject>
    struct lazy_directive : unary_generator<lazy_directive<Function, Subject> >
    {
        typedef Subject subject_type;

        template <typename Context, typename Unused>
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
            // then the expression (expr_type) is not a valid spirit karma
            // expression.
            BOOST_SPIRIT_ASSERT_MATCH(karma::domain, expr_type)

            typedef typename
                result_of::compile<karma::domain, expr_type, tag::lazy_eval>::type
            generator_type;

            typedef typename
                traits::attribute_of<generator_type, Context>::type
            type;
        };

        lazy_directive(Function const& function, Subject const& subject)
          : function(function), subject(subject) {}

        template <typename OutputIterator, typename Context, typename Delimiter
          , typename Attribute>
        bool generate(OutputIterator& sink, Context& ctx, Delimiter const& d
          , Attribute const& attr) const
        {
            return compile<karma::domain>(
                proto::make_expr<proto::tag::subscript>(
                    function(unused, ctx), subject), tag::lazy_eval())
                .generate(sink, ctx, d, attr);
        }

        template <typename Context>
        info what(Context& ctx) const
        {
            return info("lazy-directive"
              , compile<karma::domain>(
                    proto::make_expr<proto::tag::subscript>(
                        function(unused, ctx), subject), tag::lazy_eval())
                    .what(ctx)
            );
        }

        Function function;
        Subject subject;
    };

    ///////////////////////////////////////////////////////////////////////////
    // Generator generators: make_xxx function (objects)
    ///////////////////////////////////////////////////////////////////////////
    template <typename Eval, typename Modifiers>
    struct make_primitive<phoenix::actor<Eval>, Modifiers>
    {
        typedef lazy_generator<phoenix::actor<Eval> > result_type;
        result_type operator()(phoenix::actor<Eval> const& f, unused_type) const
        {
            return result_type(f);
        }
    };

    template <typename Terminal, typename Actor, int Arity, typename Modifiers>
    struct make_primitive<lazy_terminal<Terminal, Actor, Arity>, Modifiers>
    {
        typedef lazy_generator<Actor> result_type;
        result_type operator()(
            lazy_terminal<Terminal, Actor, Arity> const& lt, unused_type) const
        {
            return result_type(lt.actor);
        }
    };

    template <
        typename Terminal, typename Actor, int Arity, typename Subject
      , typename Modifiers>
    struct make_directive<lazy_terminal<Terminal, Actor, Arity>
      , Subject, Modifiers>
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
