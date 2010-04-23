/*=============================================================================
    Copyright (c) 2001-2010 Hartmut Kaiser
    Copyright (c) 2001-2010 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(SPIRIT_PASS_CONTAINER_MAR_15_2009_0114PM)
#define SPIRIT_PASS_CONTAINER_MAR_15_2009_0114PM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/karma/detail/attributes.hpp>
#include <boost/spirit/home/support/container.hpp>
#include <boost/spirit/home/support/detail/hold_any.hpp>
#include <boost/type_traits/is_base_of.hpp>
#include <boost/type_traits/is_convertible.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/or.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/repeat.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/iterator/iterator_facade.hpp>

namespace boost { namespace spirit { namespace karma { namespace detail
{
    // has_same_elements: utility to check if the LHS attribute
    // is an STL container and that its value_type is convertible
    // to the RHS.

    template <typename RHS, typename LHSAttribute
      , bool IsContainer = traits::is_container<LHSAttribute>::value>
    struct has_same_elements : mpl::false_ {};

    template <typename RHS, typename LHSAttribute>
    struct has_same_elements<RHS, LHSAttribute, true>
      : mpl::or_<
            is_convertible<RHS, typename LHSAttribute::value_type>
          , is_same<typename LHSAttribute::value_type, hold_any>
        > {};

    template <typename RHS, typename T>
    struct has_same_elements<RHS, optional<T>, true>
      : has_same_elements<RHS, T> {};

#define BOOST_SPIRIT_IS_CONVERTIBLE(z, N, data)                               \
        has_same_elements<RHS, BOOST_PP_CAT(T, N)>::value ||                  \
    /***/

    // Note: variants are treated as containers if one of the held types is a
    //       container (see support/container.hpp).
    template <typename RHS, BOOST_VARIANT_ENUM_PARAMS(typename T)>
    struct has_same_elements<
            RHS, boost::variant<BOOST_VARIANT_ENUM_PARAMS(T)>, true>
      : mpl::bool_<BOOST_PP_REPEAT(BOOST_VARIANT_LIMIT_TYPES
          , BOOST_SPIRIT_IS_CONVERTIBLE, _) false> {};

#undef BOOST_SPIRIT_IS_CONVERTIBLE

    ///////////////////////////////////////////////////////////////////////////
    // This is a wrapper for any iterator allowing to pass a reference of it
    // to the components of the sequence
    template <typename Iterator>
    class indirect_iterator
      : public boost::iterator_facade<
            indirect_iterator<Iterator>
          , typename boost::detail::iterator_traits<Iterator>::value_type
          , boost::forward_traversal_tag
          , typename boost::detail::iterator_traits<Iterator>::value_type const&>
    {
        typedef typename boost::detail::iterator_traits<Iterator>::value_type
            base_value_type;

        typedef boost::iterator_facade<
            indirect_iterator<Iterator>, base_value_type
          , boost::forward_traversal_tag, base_value_type const&
        > base_type;

    public:
        indirect_iterator()
          : iter_(0) 
        {}

        indirect_iterator(Iterator& iter)
          : iter_(&iter)
        {}

    private:
        friend class boost::iterator_core_access;

        void increment()
        {
            ++*iter_;
        }

        bool equal(indirect_iterator const& other) const
        {
            if (0 == iter_)
                return 0 == other.iter_;
            return other.iter_ != 0 && *iter_ == *other.iter_;
        }

        typename base_type::reference dereference() const
        {
            return **iter_;
        }

    private:
        Iterator* iter_;
    };

    template <typename Iterator>
    struct make_indirect_iterator
    {
        typedef indirect_iterator<Iterator> type;
    };

    template <>
    struct make_indirect_iterator<unused_type const*>
    {
        typedef unused_type const* type;
    };

    ///////////////////////////////////////////////////////////////////////////
    // This function handles the case where the attribute (Attr) given
    // to the sequence is an STL container. This is a wrapper around F.
    // The function F does the actual generating.
    template <typename F, typename Attr, typename Strict>
    struct pass_container
    {
        typedef typename F::context_type context_type;
        typedef typename traits::container_iterator<Attr const>::type 
            iterator_type;

        pass_container(F const& f, Attr const& attr)
          : f(f), attr(attr), iter(traits::begin(attr)) {}

        // this is for the case when the current element expects an attribute
        // which is taken from the next entry in the container
        template <typename Component>
        bool dispatch_attribute_element(Component const& component, mpl::false_) const
        {
            // get the next value to generate from container
            iterator_type end = traits::end(attr);
            if (!traits::compare(iter, end) && !f(component, traits::deref(iter))) 
            {
                // needs to return false as long as everything is ok
                traits::next(iter);
                return false;
            }
            // either no elements available any more or generation failed
            return true;
        }

        // this is for the case when the current element expects an attribute
        // which is a container itself, this element will get the rest of the 
        // attribute container
        template <typename Component>
        bool dispatch_attribute_element(Component const& component, mpl::true_) const
        {
            typedef typename make_indirect_iterator<iterator_type>::type 
                indirect_iterator_type;

            iterator_type end = traits::end(attr);
            indirect_iterator_type ind_iter(iter);
            indirect_iterator_type ind_end(end);

            return f(component, make_iterator_range(ind_iter, ind_end));
        }

        // This handles the distinction between elements in a sequence expecting
        // containers themselves and elements expecting non-containers as their 
        // attribute. Note: is_container treats optional<T>, where T is a 
        // container as a container as well.
        template <typename Component>
        bool dispatch_attribute(Component const& component, mpl::true_) const
        {
            typedef typename traits::attribute_of<
                Component, context_type>::type attribute_type;

            typedef mpl::and_<
                traits::is_container<attribute_type>
              , is_convertible<Attr, attribute_type> > predicate;

            return dispatch_attribute_element(component, predicate());
        }

        // this is for the case when the current element doesn't expect an 
        // attribute
        template <typename Component>
        bool dispatch_attribute(Component const& component, mpl::false_) const
        {
            return f(component, unused);
        }

        // This handles the case where the attribute of the component
        // is not a STL container or which elements are not 
        // convertible to the target attribute (Attr) value_type.
        template <typename Component>
        bool dispatch_main(Component const& component, mpl::false_) const
        {
            // we need to dispatch again depending on the type of the attribute
            // of the current element (component). If this is has no attribute
            // we shouldn't use an element of the container but unused_type 
            // instead
            typedef traits::not_is_unused<
                typename traits::attribute_of<Component, context_type>::type
            > predicate;

            return dispatch_attribute(component, predicate());
        }

        // This handles the case where the attribute of the component is
        // an STL container *and* its value_type is convertible to the
        // target attribute's (Attr) value_type.
        template <typename Component>
        bool dispatch_main(Component const& component, mpl::true_) const
        {
            typedef typename make_indirect_iterator<iterator_type>::type 
                indirect_iterator_type;

            iterator_type end = traits::end(attr);
            indirect_iterator_type ind_iter(iter);
            indirect_iterator_type ind_end(end);

            return f(component, make_iterator_range(ind_iter, ind_end));
        }

        // Dispatches to dispatch_main depending on the attribute type
        // of the Component
        template <typename Component>
        bool operator()(Component const& component) const
        {
            typedef typename traits::container_value<Attr>::type rhs;
            typedef typename traits::attribute_of<
                Component, context_type>::type lhs_attribute;

            // false means everything went ok
            return dispatch_main(component
              , has_same_elements<rhs, lhs_attribute>());
        }

        F f;
        Attr const& attr;
        mutable iterator_type iter;

    private:
        // silence MSVC warning C4512: assignment operator could not be generated
        pass_container& operator= (pass_container const&);
    };
}}}}

#endif
