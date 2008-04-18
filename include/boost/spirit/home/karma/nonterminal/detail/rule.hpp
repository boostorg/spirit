//  Copyright (c) 2001-2007 Joel de Guzman
//  Copyright (c) 2001-2008 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(BOOST_SPIRIT_KARMA_RULE_MAR_05_2007_0519PM)
#define BOOST_SPIRIT_KARMA_RULE_MAR_05_2007_0519PM

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once      // MS compatible compilers support #pragma once
#endif

#include <boost/spirit/home/support/unused.hpp>
#include <boost/spirit/home/support/attribute_of.hpp>
#include <boost/spirit/home/karma/domain.hpp>
#include <boost/intrusive_ptr.hpp>
#include <boost/detail/atomic_count.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/not.hpp>
#include <boost/mpl/and.hpp>
#include <boost/mpl/equal_to.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/type_traits/add_const.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/function_types/is_function.hpp>
#include <boost/assert.hpp>
#include <boost/fusion/include/pop_front.hpp>
#include <boost/fusion/include/is_sequence.hpp>

namespace boost { namespace spirit { namespace karma { namespace detail
{
    template <typename OutputIterator, typename Context, typename Delimiter>
    struct virtual_component_base
    {
        struct take_no_delimiter {};

        typedef typename
            mpl::eval_if<
                is_same<Delimiter, unused_type>,
                mpl::identity<take_no_delimiter>,
                result_of::as_component<karma::domain, Delimiter>
            >::type
        delimiter_type;

        virtual_component_base()
          : use_count(0)
        {
        }

        virtual ~virtual_component_base()
        {
        }

        virtual bool
        generate(OutputIterator& sink, Context& context,
            delimiter_type const& delim) = 0;

        virtual bool
        generate(OutputIterator& sink, Context& context, unused_type) = 0;

        boost::detail::atomic_count use_count;
    };

    template <typename OutputIterator, typename Context, typename Delimiter>
    inline void
    intrusive_ptr_add_ref(
        virtual_component_base<OutputIterator, Context, Delimiter>* p)
    {
        ++p->use_count;
    }

    template <typename OutputIterator, typename Context, typename Delimiter>
    inline void
    intrusive_ptr_release(
        virtual_component_base<OutputIterator, Context, Delimiter>* p)
    {
        if (--p->use_count == 0)
            delete p;
    }

    ///////////////////////////////////////////////////////////////////////////
    template <typename Component, typename Context>
    struct needs_single_attribute
    :   mpl::not_<
            typename fusion::traits::is_sequence<
                typename traits::attribute_of<
                    karma::domain, Component, Context>::type
            >::type
        >
    {
    };

    ///////////////////////////////////////////////////////////////////////////
    template <typename MustDeref, typename Parameter>
    struct deref_if
    {
        typedef typename
            mpl::eval_if<
                MustDeref,
                fusion::result_of::at_c<Parameter, 1>,
                fusion::result_of::pop_front<Parameter>
            >::type
        type;

        template <typename Param>
        static type
        call(Param const& param, mpl::true_)
        {
            return fusion::at_c<1>(param);
        }

        template <typename Param>
        static type
        call(Param const& param, mpl::false_)
        {
            return fusion::pop_front(param);
        }

        template <typename Param>
        static type
        call(Param const& param)
        {
            return call(param, MustDeref());
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    template <typename Component, typename Context>
    struct propagate_param
    {
        // If first element of the context (that's the parameter type) contains
        // one element only, then the parameter type to propagate is unused,
        // otherwise we consider to use everything except the first element of
        // this sequence.
        //
        // If the resulting sequence type is a fusion sequence containing
        // exactly one element and the right hand side expects a singular
        // (non-sequence) parameter we pass on the first (and only) element of
        // this sequence, otherwise we pass the parameter sequence itself.
        typedef typename
            boost::add_const<
                typename boost::remove_reference<
                    typename fusion::result_of::at_c<Context, 0>::type
                >::type
            >::type
        parameter_type;

        // this evaluates to mpl::true_ if the rules parameter type is unused
        typedef typename
            mpl::equal_to<mpl::size<parameter_type>, mpl::long_<1> >::type
        no_parameter;

        // this evaluates to mpl::true_ if the rules parameter type contains
        // one element and the right hand side generator expects a single
        // (non-sequence) parameter type
        typedef typename
            mpl::and_<
                mpl::equal_to<mpl::size<parameter_type>, mpl::long_<2> >,
                needs_single_attribute<Component, Context>
            >::type
        must_dereference;

        typedef typename
            mpl::eval_if<
                no_parameter,
                mpl::identity<unused_type const>,
                deref_if<must_dereference, parameter_type>
            >::type
        propagated_type;

        template <typename Ctx>
        static unused_type const
        call(Ctx& context, mpl::true_)
        {
            return unused;
        }

        template <typename Ctx>
        static propagated_type
        call(Ctx& context, mpl::false_)
        {
            return deref_if<must_dereference, parameter_type>::
                call(fusion::at_c<0>(context));
        }

        template <typename Ctx>
        static propagated_type
        call(Ctx& context)
        {
            return call(context, no_parameter());
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    template <typename OutputIterator, typename Component, typename Context,
        typename Delimiter, typename Auto>
    struct virtual_component
      : virtual_component_base<OutputIterator, Context, Delimiter>
    {
        typedef
            virtual_component_base<OutputIterator, Context, Delimiter>
        base_type;
        typedef typename base_type::delimiter_type delimiter_type;
        typedef typename base_type::take_no_delimiter take_no_delimiter;

        virtual_component(Component const& component)
          : component(component)
        {
        }

        virtual ~virtual_component()
        {
        }

        template <typename Delimiter_>
        bool generate_main(OutputIterator& sink, Context& context,
            Delimiter_ const& delim, mpl::false_)
        {
            // If Auto is false, the component's parameter is unused.
            typedef typename Component::director director;
            return director::generate(component, sink, context, delim, unused);
        }

        template <typename Delimiter_>
        bool generate_main(OutputIterator& sink, Context& context,
            Delimiter_ const& delim, mpl::true_)
        {
            //  If Auto is true, we pass the rule's parameters on to the
            //  component.
            typedef typename Component::director director;
            return director::generate(component, sink, context, delim,
                detail::propagate_param<Component, Context>::call(context));
        }

        bool
        generate_main(OutputIterator& /*sink*/, Context& /*context*/, take_no_delimiter,
            mpl::false_)
        {
            BOOST_ASSERT(false); // this should never be called
            return false;
        }

        bool
        generate_main(OutputIterator& /*sink*/, Context& /*context*/, take_no_delimiter,
            mpl::true_)
        {
            BOOST_ASSERT(false); // this should never be called
            return false;
        }

        virtual bool
        generate(OutputIterator& sink, Context& context,
            delimiter_type const& delim)
        {
            return generate_main(sink, context, delim, Auto());
        }

        virtual bool
        generate(OutputIterator& sink, Context& context, unused_type)
        {
            return generate_main(sink, context, unused, Auto());
        }

        Component component;
    };

}}}}

#endif
