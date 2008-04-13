//  Copyright (c) 2001-2007 Joel de Guzman
//  Copyright (c) 2001-2008 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(BOOST_SPIRIT_KARMA_NONTERMINAL_DIRECTOR_MAR_05_2007_0524PM)
#define BOOST_SPIRIT_KARMA_NONTERMINAL_DIRECTOR_MAR_05_2007_0524PM

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once      // MS compatible compilers support #pragma once
#endif

#include <boost/spirit/home/support/nonterminal/nonterminal.hpp>
#include <boost/spirit/home/support/nonterminal/detail/expand_arg.hpp>
#include <boost/spirit/home/karma/domain.hpp>
#include <boost/spirit/home/support/component.hpp>
#include <boost/spirit/home/support/detail/values.hpp>
#include <boost/fusion/include/transform.hpp>
#include <boost/fusion/include/join.hpp>
#include <boost/fusion/include/single_view.hpp>
#include <boost/intrusive_ptr.hpp>
#include <boost/mpl/at.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/type_traits/remove_const.hpp>

namespace boost { namespace spirit { namespace karma
{
    struct nonterminal_director
    {
        template <typename Component, typename Context, typename Unused>
        struct attribute
        {
            typedef typename result_of::subject<Component>::type nonterminal_holder;
            typedef typename nonterminal_holder::nonterminal_type::param_types type;
        };

        template <
            typename NonterminalContext, typename Nonterminal,
            typename OutputIterator, typename Context,
            typename Delimiter, typename Parameter>
        static bool generate_nonterminal(
            nonterminal_object<Nonterminal> const& x,
            OutputIterator& sink, Context& context_,
            Delimiter const& delim, Parameter const& param)
        {
            // the nonterminal_holder holds an actual nonterminal_object
            typedef typename Nonterminal::locals_type locals_type;
            NonterminalContext context(param, locals_type());
            return x.obj.generate(sink, context, delim);
        }

        template <
            typename NonterminalContext, typename Nonterminal,
            typename OutputIterator, typename Context,
            typename Delimiter, typename Parameter>
        static bool generate_nonterminal(
            Nonterminal const* ptr,
            OutputIterator& sink, Context& /*context_*/,
            Delimiter const& delim, Parameter const& param)
        {
            // the nonterminal_holder holds a pointer to a nonterminal
            typedef typename Nonterminal::locals_type locals_type;
            NonterminalContext context(param, locals_type());
            return ptr->generate(sink, context, delim);
        }

        template <
            typename NonterminalContext, typename Nonterminal,
            typename FSequence, typename OutputIterator,
            typename Context, typename Delimiter,
            typename Parameter>
        static bool generate_nonterminal(
            parameterized_nonterminal<Nonterminal, FSequence> const& x,
            OutputIterator& sink, Context& context_,
            Delimiter const& delim, Parameter const& /*param*/)
        {
            // the nonterminal_holder holds a parameterized_nonterminal
            typedef typename Nonterminal::locals_type locals_type;
            fusion::single_view<unused_type const> front(unused);
            NonterminalContext context(
                fusion::join(
                    front,
                    fusion::transform(
                        x.fseq,
                        spirit::detail::expand_arg<Context>(context_)
                    )
                ),
                locals_type()
            );
            return x.ptr->generate(sink, context, delim);
        }

        ///////////////////////////////////////////////////////////////////////
        // main entry point
        ///////////////////////////////////////////////////////////////////////
        template <
            typename Component,
            typename OutputIterator, typename Context,
            typename Delimiter, typename Parameter>
        static bool generate(
            Component const& component, OutputIterator& sink,
            Context& context, Delimiter const& delim, Parameter const& param)
        {
            typedef typename
                result_of::subject<Component>::type
            nonterminal_holder;
            
            //  The overall context_type consists of a tuple with:
            //      1) a tuple of the return value and parameters
            //      2) the locals
            //  if no signature is specified the first tuple contains
            //  an unused_type element at position zero only.

            typedef typename 
                nonterminal_holder::nonterminal_type::context_type 
            context_type;

            typedef typename
                mpl::if_<
                    is_same<typename remove_const<Parameter>::type, unused_type>,
                    context_type,
                    Parameter
                >::type
            parameter_type;

            // create an parameter if one is not supplied
            parameter_type p (spirit::detail::make_value<parameter_type>::call(param));

            return generate_nonterminal<context_type>(
                subject(component).held, sink, context, delim, p
            );
        }

        template <typename Nonterminal>
        static std::string what_nonterminal(nonterminal_object<Nonterminal> const& x)
        {
            // the nonterminal_holder holds an actual nonterminal_object
            return x.obj.what();
        }

        template <typename Nonterminal>
        static std::string what_nonterminal(Nonterminal const* ptr)
        {
            // the nonterminal_holder holds a pointer to a nonterminal
            return ptr->what();
        }

        template <typename Nonterminal, typename FSequence>
        static std::string what_nonterminal(
            parameterized_nonterminal<Nonterminal, FSequence> const& x)
        {
            // the nonterminal_holder holds a parameterized_nonterminal
            return x.ptr->what();
        }

        template <typename Component>
        static std::string what(Component const& component)
        {
            return what_nonterminal(subject(component).held);
        }
    };

}}}

#endif
