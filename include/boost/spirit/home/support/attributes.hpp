/*=============================================================================
    Copyright (c) 2001-2009 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_SPIRIT_ATTRIBUTES_JANUARY_29_2007_0954AM)
#define BOOST_SPIRIT_ATTRIBUTES_JANUARY_29_2007_0954AM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/support/unused.hpp>
#include <boost/spirit/home/support/detail/as_variant.hpp>
#include <boost/optional/optional.hpp>
#include <boost/fusion/include/transform.hpp>
#include <boost/fusion/include/filter_if.hpp>
#include <boost/fusion/include/as_vector.hpp>
#include <boost/fusion/include/push_front.hpp>
#include <boost/fusion/include/is_sequence.hpp>
#include <boost/utility/value_init.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/end.hpp>
#include <boost/mpl/equal_to.hpp>
#include <boost/mpl/find_if.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/variant.hpp>
#include <vector>
#include <utility>

namespace boost { namespace spirit { namespace traits
{
    ///////////////////////////////////////////////////////////////////////////
    // This file deals with attribute related functions and metafunctions
    // including generalized attribute transformation utilities for Spirit
    // components.
    ///////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////
    // attribute_of
    //
    // Get the component's attribute
    ///////////////////////////////////////////////////////////////////////////
    template <typename Component
      , typename Context = unused_type, typename Iterator = unused_type>
    struct attribute_of
    {
        typedef typename Component::template
            attribute<Context, Iterator>::type type;
    };

    ///////////////////////////////////////////////////////////////////////////
    // attribute_not_unused
    //
    // An mpl metafunction class that determines whether a component's
    // attribute is not unused.
    ///////////////////////////////////////////////////////////////////////////
    template <typename Context, typename Iterator = unused_type>
    struct attribute_not_unused
    {
        template <typename Component>
        struct apply
          : is_not_unused<typename
                attribute_of<Component, Context, Iterator>::type>
        {};
    };

    ///////////////////////////////////////////////////////////////////////////
    // pass_attribute
    //
    // Determines how we pass attributes to semantic actions. This
    // may be specialized. By default, all attributes are wrapped in
    // a fusion sequence, because the attribute has to be treated as being
    // a single value in any case (even if it actually already is a fusion
    // sequence in its own).
    ///////////////////////////////////////////////////////////////////////////
    template <typename Component, typename Attribute, typename Enable = void>
    struct pass_attribute
    {
        typedef fusion::vector1<Attribute&> type;
    };

    ///////////////////////////////////////////////////////////////////////////
    // Subclass a pass_attribute specialization from this to wrap
    // the attribute in a tuple only IFF it is not already a fusion tuple.
    ///////////////////////////////////////////////////////////////////////////
    template <typename Attribute>
    struct wrap_if_not_tuple
    {
        typedef typename
            mpl::if_<
                fusion::traits::is_sequence<Attribute>
              , Attribute&
              , fusion::vector1<Attribute&>
            >::type
        type;
    };

    template <>
    struct wrap_if_not_tuple<unused_type>
    {
        typedef unused_type type;
    };

    template <>
    struct wrap_if_not_tuple<unused_type const>
    {
        typedef unused_type type;
    };

    ///////////////////////////////////////////////////////////////////////////
    // build_optional
    //
    // Build a boost::optional from T. Return unused_type if T is unused_type.
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    struct build_optional
    {
        typedef optional<T> type;
    };

    template <>
    struct build_optional<unused_type>
    {
        typedef unused_type type;
    };

    ///////////////////////////////////////////////////////////////////////////
    // build_std_vector
    //
    // Build a std::vector from T. Return unused_type if T is unused_type.
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    struct build_std_vector
    {
        typedef std::vector<T> type;
    };

    template <>
    struct build_std_vector<unused_type>
    {
        typedef unused_type type;
    };

    ///////////////////////////////////////////////////////////////////////////
    // filter_unused_attributes
    //
    // Remove unused_types from a sequence
    ///////////////////////////////////////////////////////////////////////////
    template <typename Sequence>
    struct filter_unused_attributes
    {
        // Compute the list of all *used* attributes of sub-components
        // (filter all unused attributes from the list)
        typedef typename fusion::result_of::filter_if<
                Sequence
              , is_not_unused<mpl::_>
            >::type
        type;
    };

    ///////////////////////////////////////////////////////////////////////////
    // build_fusion_vector
    //
    // Build a fusion vector from a fusion sequence. All unused attributes
    // are filtered out. If the result is empty after the removal of unused
    // types, return unused_type. If the input sequence is an unused_type,
    // also return unused_type.
    ///////////////////////////////////////////////////////////////////////////
    template <typename Sequence>
    struct build_fusion_vector
    {
        // Remove all unused attributes
        typedef typename
            filter_unused_attributes<Sequence>::type
        filtered_attributes;

        // Build a fusion vector from a fusion sequence (Sequence),
        // But *only if* the sequence is not empty. i.e. if the
        // sequence is empty, our result will be unused_type.

        typedef typename
            mpl::eval_if<
                fusion::result_of::empty<filtered_attributes>
              , mpl::identity<unused_type>
              , fusion::result_of::as_vector<filtered_attributes>
            >::type
        type;
    };

    template <>
    struct build_fusion_vector<unused_type>
    {
        typedef unused_type type;
    };

    ///////////////////////////////////////////////////////////////////////////
    // build_attribute_sequence
    //
    // Build a fusion sequence attribute sequence from a sequence of
    // components. Transform<T>::type is called on each element.
    ///////////////////////////////////////////////////////////////////////////
    template <typename Sequence, typename Context
      , template <typename T> class Transform, typename Iterator = unused_type>
    struct build_attribute_sequence
    {
        struct element_attribute
        {
            template <typename T>
            struct result;

            template <typename F, typename Element>
            struct result<F(Element)>
            {
                typedef typename
                    Transform<
                        typename attribute_of<Element, Context, Iterator>::type
                    >::type
                type;
            };
        };

        // Compute the list of attributes of all sub-components
        typedef typename
            fusion::result_of::transform<Sequence, element_attribute>::type
        type;
    };

    ///////////////////////////////////////////////////////////////////////////
    // has_no_unused
    //
    // Test if there are no unused attributes in Sequence
    ///////////////////////////////////////////////////////////////////////////
    template <typename Sequence>
    struct has_no_unused
      : is_same<
            typename mpl::find_if<Sequence, is_same<mpl::_, unused_type> >::type
          , typename mpl::end<Sequence>::type>
    {};

    ///////////////////////////////////////////////////////////////////////////
    // build_variant
    //
    // Build a boost::variant from a fusion sequence. build_variant makes sure
    // that 1) all attributes in the variant are unique 2) puts the unused
    // attribute, if there is any, to the front and 3) collapses single element
    // variants, variant<T> to T.
    ///////////////////////////////////////////////////////////////////////////
    template <typename Sequence>
    struct build_variant
    {
        // Remove all unused attributes.
        typedef typename
            filter_unused_attributes<Sequence>::type
        filtered_attributes;

        typedef has_no_unused<Sequence> has_no_unused_type;

        // If the original attribute list does not contain any unused
        // attributes, it is used, otherwise a single unused_type is
        // pushed to the front of the list. This is to make sure that if
        // there is an unused_type in the list, it is the first one.
        typedef typename
            mpl::eval_if<
                has_no_unused_type
              , mpl::identity<Sequence>
              , fusion::result_of::push_front<filtered_attributes, unused_type>
            >::type
        attribute_sequence;

        // Make sure each of the types occur only once in the type list
        typedef typename
            mpl::fold<
                attribute_sequence, mpl::vector<>,
                mpl::if_<
                    mpl::contains<mpl::_1, mpl::_2>,
                    mpl::_1, mpl::push_back<mpl::_1, mpl::_2>
                >
            >::type
        new_sequence;

        // If there is only an unused_type and one other type in the list of 
        // types we strip off the variant. IOTW, we collapse single element 
        // variants, variant<unused_type, T> to T.
        typedef typename
            mpl::eval_if<
                mpl::and_<
                    mpl::equal_to<mpl::size<new_sequence>, mpl::int_<2> >
                  , mpl::not_<has_no_unused_type> >
              , fusion::result_of::pop_front<new_sequence>
              , mpl::identity<new_sequence>
            >::type
        shortened_type;

        // If there is only one type in the list of types we strip off the
        // variant as well. IOTW, we collapse single element variants, 
        // variant<T> to T.
        typedef typename
            mpl::eval_if<
                mpl::equal_to<mpl::size<shortened_type>, mpl::int_<1> >,
                mpl::front<shortened_type>,
                spirit::detail::as_variant<shortened_type>
            >::type
        type;
    };

    ///////////////////////////////////////////////////////////////////////////
    // make_attribute
    //
    // All parsers and generators have specific attribute or parameter types.
    // Spirit parsers are passed an attribute and Spirit generators
    // are passed a parameter; these are either references to the expected
    // type, or an unused_type -- to flag that we do not care about the
    // attribute/parameter. For semantic actions, however, we need to have a
    // real value to pass to the semantic action. If the client did not
    // provide one, we will have to synthesize the value. This class
    // takes care of that.
    ///////////////////////////////////////////////////////////////////////////
    template <typename Attribute, typename ActualAttribute>
    struct make_attribute
    {
        typedef typename
            mpl::if_<
                is_same<typename remove_const<ActualAttribute>::type, unused_type>
              , typename remove_const<Attribute>::type
              , ActualAttribute&>::type
        type;

        typedef typename
            mpl::if_<
                is_same<typename remove_const<ActualAttribute>::type, unused_type>
              , typename remove_const<Attribute>::type
              , ActualAttribute>::type
        value_type;

        static Attribute call(unused_type)
        {
             // synthesize the attribute/parameter
            return boost::get(value_initialized<Attribute>());
        }

        template <typename T>
        static T& call(T& value)
        {
            return value; // just pass the one provided
        }

        template <typename T>
        static T const& call(T const& value)
        {
            return value; // just pass the one provided
        }
    };

    template <typename Attribute, typename ActualAttribute>
    struct make_attribute<Attribute&, ActualAttribute>
      : make_attribute<Attribute, ActualAttribute>
    {
    };

    template <typename Attribute, typename ActualAttribute>
    struct make_attribute<Attribute const&, ActualAttribute>
      : make_attribute<Attribute, ActualAttribute>
    {
    };

    template <typename ActualAttribute>
    struct make_attribute<unused_type, ActualAttribute>
    {
        typedef unused_type type;
        static unused_type call(unused_type)
        {
            return unused;
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    // swap_impl
    //
    // Swap (with proper handling of unused_types)
    ///////////////////////////////////////////////////////////////////////////
    template <typename A, typename B>
    void swap_impl(A& a, B& b)
    {
        A temp = a;
        a = b;
        b = temp;
    }

    template <typename T>
    void swap_impl(T& a, T& b)
    {
        using namespace std;
        swap(a, b);
    }

    template <typename A>
    void swap_impl(A& a, unused_type)
    {
    }

    template <typename A>
    void swap_impl(unused_type, A& a)
    {
    }

    inline void swap_impl(unused_type, unused_type)
    {
    }

    ///////////////////////////////////////////////////////////////////////////
    //  Strips single element fusion vectors into its 'naked'
    //  form: vector<T> --> T
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    struct strip_single_element_vector
    {
        typedef T type;
    };

    template <typename T>
    struct strip_single_element_vector<fusion::vector1<T> >
    {
        typedef T type;
    };

    template <typename T>
    struct strip_single_element_vector<fusion::vector<T> >
    {
        typedef T type;
    };

}}}

#endif
