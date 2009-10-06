//  Copyright (c) 2001-2009 Hartmut Kaiser
//  Copyright (c) 2001-2009 Joel de Guzman
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(SPIRIT_KARMA_ALTERNATIVE_MAR_01_2007_1124AM)
#define SPIRIT_KARMA_ALTERNATIVE_MAR_01_2007_1124AM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/karma/domain.hpp>
#include <boost/spirit/home/support/unused.hpp>
#include <boost/spirit/home/support/attributes.hpp>
#include <boost/spirit/home/support/detail/hold_any.hpp>
#include <boost/spirit/home/karma/detail/output_iterator.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/mpl/find_if.hpp>
#include <boost/mpl/deref.hpp>
#include <boost/mpl/distance.hpp>
#include <boost/mpl/or.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/type_traits/is_convertible.hpp>
#include <boost/variant.hpp>

///////////////////////////////////////////////////////////////////////////////
namespace boost { namespace spirit { namespace karma { namespace detail
{
    ///////////////////////////////////////////////////////////////////////////
    //  A component is compatible to a given Attribute type if the Attribute
    //  is the same as the expected type of the component
    ///////////////////////////////////////////////////////////////////////////
    template <typename Expected, typename Attribute>
    struct attribute_is_compatible 
      : is_convertible<Attribute, Expected> {};

    template <typename Expected, typename Attribute>
    struct attribute_is_compatible<Expected, boost::optional<Attribute> >
      : is_convertible<Attribute, Expected> {};

    template <typename Expected, typename Attribute, typename IsNotVariant>
    struct compute_compatible_component_variant
      : mpl::or_<
          attribute_is_compatible<Expected, Attribute>
        , is_same<hold_any, Expected> > {};

    template <typename Expected, typename Attribute>
    struct compute_compatible_component_variant<Expected, Attribute, mpl::false_>
    {
        typedef typename Attribute::types types;
        typedef typename mpl::end<types>::type end;

        typedef typename 
            mpl::find_if<types, is_same<Expected, mpl::_1> >::type 
        iter;

        typedef typename mpl::distance<
            typename mpl::begin<types>::type, iter
        >::type distance;

        typedef typename mpl::not_<is_same<iter, end> >::type type;
        enum { value = type::value };
    };

    template <typename Expected, typename Attribute>
    struct compute_compatible_component
      : compute_compatible_component_variant<Expected, Attribute
          , typename spirit::traits::not_is_variant<Attribute>::type> {};

    template <typename Expected>
    struct compute_compatible_component<Expected, unused_type>
      : mpl::false_ {};

    template <typename Attribute>
    struct compute_compatible_component<unused_type, Attribute>
      : mpl::false_ {};

    template <>
    struct compute_compatible_component<unused_type, unused_type>
      : mpl::false_ {};

    ///////////////////////////////////////////////////////////////////////////
    //  execute a generator if the given Attribute type is compatible
    ///////////////////////////////////////////////////////////////////////////

    //  this gets instantiated if the Attribute type is _not_ compatible with
    //  the generator
    template <typename Component, typename Attribute, typename Expected
      , typename Enable = void>
    struct alternative_generate
    {
        template <typename OutputIterator, typename Context, typename Delimiter>
        static bool
        call(Component const&, OutputIterator&, Context&, Delimiter const&
          , Attribute const&)
        {
            return false;
        }
    };

    template <typename Component>
    struct alternative_generate<Component, unused_type, unused_type>
    {
        template <typename OutputIterator, typename Context, typename Delimiter>
        static bool
        call(Component const& component, OutputIterator& sink, Context& ctx
          , Delimiter const& d, unused_type)
        {
            // return true if any of the generators succeed
            return component.generate(sink, ctx, d, unused);
        }
    };

    //  this gets instantiated if there is no Attribute given for the
    //  alternative generator
    template <typename Component, typename Expected>
    struct alternative_generate<Component, unused_type, Expected>
      : alternative_generate<Component, unused_type, unused_type> {};

    //  this gets instantiated if the generator does not expect to receive an
    //  Attribute (the generator is self contained).
    template <typename Component, typename Attribute>
    struct alternative_generate<Component, Attribute, unused_type>
      : alternative_generate<Component, unused_type, unused_type> {};

    //  this gets instantiated if the Attribute type is compatible to the
    //  generator
    template <typename Component, typename Attribute, typename Expected>
    struct alternative_generate<Component, Attribute, Expected
      , typename enable_if<
            compute_compatible_component<Expected, Attribute> >::type>
    {
        template <typename OutputIterator, typename Context, typename Delimiter>
        static bool
        call(Component const& component, OutputIterator& sink
          , Context& ctx, Delimiter const& d, Attribute const& attr)
        {
            return call(component, sink, ctx, d, attr
              , spirit::traits::not_is_variant<Attribute>());
        }

        template <typename OutputIterator, typename Context, typename Delimiter>
        static bool
        call(Component const& component, OutputIterator& sink
          , Context& ctx, Delimiter const& d, Attribute const& attr, mpl::true_)
        {
            return component.generate(sink, ctx, d, attr);
        }

        template <typename OutputIterator, typename Context, typename Delimiter>
        static bool
        call(Component const& component, OutputIterator& sink
          , Context& ctx, Delimiter const& d, Attribute const& attr, mpl::false_)
        {
            typedef
                compute_compatible_component<Expected, Attribute>
            component_type;

            typedef typename component_type::distance distance_type;

            // make sure, the content of the passed variant matches our
            // expectations
            if (attr.which() != distance_type::value)
                return false;

            // returns true if any of the generators succeed
            typedef
                typename mpl::deref<typename component_type::iter>::type
            compatible_type;

            return component.generate(sink, ctx, d, get<compatible_type>(attr));
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    //  alternative_generate_functor: a functor supplied to fusion::any which
    //  will be executed for every generator in a given alternative generator
    //  expression
    ///////////////////////////////////////////////////////////////////////////
    template <typename OutputIterator, typename Context, typename Delimiter,
        typename Attribute>
    struct alternative_generate_functor
    {
        alternative_generate_functor(OutputIterator& sink_, Context& ctx_
            , Delimiter const& d, Attribute const& attr_)
          : sink(sink_), ctx(ctx_), delim(d), attr(attr_) {}

        template <typename Component>
        bool operator()(Component const& component)
        {
            typedef
                typename traits::attribute_of<Component, Context>::type
            expected_type;
            typedef
                alternative_generate<Component, Attribute, expected_type>
            generate;

            // wrap the given output iterator avoid output as long as one
            // component fails
            detail::enable_buffering<OutputIterator> buffering(sink);
            bool r = false;
            {
                detail::disable_counting<OutputIterator> nocounting(sink);
                r = generate::call(component, sink, ctx, delim, attr);
            }
            if (r) 
                buffering.buffer_copy();
            return r;
        }

        OutputIterator& sink;
        Context& ctx;
        Delimiter const& delim;
        Attribute const& attr;
    };

}}}}

#endif
