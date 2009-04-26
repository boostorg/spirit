/*=============================================================================
    Copyright (c) 2001-2009 Joel de Guzman
    Copyright (c) 2001-2009 Hartmut Kaiser
    http://spirit.sourceforge.net/

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_CONTAINER_FEBRUARY_06_2007_1001AM)
#define BOOST_SPIRIT_CONTAINER_FEBRUARY_06_2007_1001AM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/support/unused.hpp>
#include <boost/detail/iterator.hpp> // for boost::detail::iterator_traits
#include <boost/mpl/has_xxx.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/optional.hpp>
//#include <boost/variant.hpp>
//#include <boost/preprocessor/cat.hpp>
//#include <boost/preprocessor/repeat.hpp>

namespace boost { namespace spirit { namespace traits
{
    ///////////////////////////////////////////////////////////////////////////
    //  This file contains some container utils for stl containers. The
    //  utilities provided also accept spirit's unused_type; all no-ops.
    //  Compiler optimization will easily strip these away.
    ///////////////////////////////////////////////////////////////////////////

    namespace detail
    {
        BOOST_MPL_HAS_XXX_TRAIT_DEF(value_type);
        BOOST_MPL_HAS_XXX_TRAIT_DEF(iterator);
        BOOST_MPL_HAS_XXX_TRAIT_DEF(size_type);
        BOOST_MPL_HAS_XXX_TRAIT_DEF(reference);
    }

    template <typename T>
    struct is_container :
        mpl::bool_<
            detail::has_value_type<T>::value &&
            detail::has_iterator<T>::value &&
            detail::has_size_type<T>::value &&
            detail::has_reference<T>::value
        >
    {};

//    template <typename T>
//    struct is_container<optional<T> > 
//      : is_container<T> {};

//#define BOOST_SPIRIT_IS_CONTAINER(z, N, data)                                 \
//        is_container<BOOST_PP_CAT(T, N)>::value ||                            \
//    /***/

//    template <BOOST_VARIANT_ENUM_PARAMS(typename T)>
//    struct is_container<boost::variant<BOOST_VARIANT_ENUM_PARAMS(T)> > 
//      : mpl::bool_<BOOST_PP_REPEAT(BOOST_VARIANT_LIMIT_TYPES
//          , BOOST_SPIRIT_IS_CONTAINER, _) false> {};

//#undef BOOST_SPIRIT_IS_CONTAINER

    ///////////////////////////////////////////////////////////////////////////
//    template <typename T>
//    struct container_type 
//    {
//        typedef T type;
//    };

//    template <typename T>
//    struct container_type<optional<T> > 
//      : container_type<T> {};

//    template <BOOST_VARIANT_ENUM_PARAMS(typename T)>
//    struct container_type<boost::variant<BOOST_VARIANT_ENUM_PARAMS(T)> > 
//    {
//        typedef typename 
//            boost::variant<BOOST_VARIANT_ENUM_PARAMS(T)>::types 
//        types;

//        typedef typename mpl::find_if<types, is_container<mpl::_1> >::type iter;
//        typedef typename mpl::deref<iter>::type type;
//    };

    ///////////////////////////////////////////////////////////////////////////
    namespace result_of
    {
        template <typename Container>
        struct value
        {
            typedef typename Container::value_type type;
        };

        template <>
        struct value<unused_type>
        {
            typedef unused_type type;
        };

        template <>
        struct value<unused_type const>
        {
            typedef unused_type type;
        };

        template <typename Container>
        struct iterator
        {
            typedef typename Container::iterator type;
        };

        template <typename Container>
        struct iterator<Container const>
        {
            typedef typename Container::const_iterator type;
        };

        template <>
        struct iterator<unused_type>
        {
            typedef unused_type const* type;
        };

        template <>
        struct iterator<unused_type const>
        {
            typedef unused_type const* type;
        };

        template <typename T>
        struct optional_value
        {
            typedef T type;
        };

        template <typename T>
        struct optional_value<optional<T> >
        {
            typedef T type;
        };

        template <>
        struct optional_value<unused_type>
        {
            typedef unused_type type;
        };

        template <>
        struct optional_value<unused_type const>
        {
            typedef unused_type type;
        };
    }

    ///////////////////////////////////////////////////////////////////////////
    template <typename Container, typename T>
    inline void push_back(Container& c, T const& val)
    {
        c.push_back(val);
    }

    template <typename Container>
    inline void push_back(Container&, unused_type)
    {
    }

    template <typename T>
    inline void push_back(unused_type, T const&)
    {
    }

    inline void push_back(unused_type, unused_type)
    {
    }

    ///////////////////////////////////////////////////////////////////////////
    template <typename Container>
    inline typename result_of::iterator<Container>::type
    begin(Container& c)
    {
        return c.begin();
    }

    template <typename Container>
    inline typename result_of::iterator<Container const>::type
    begin(Container const& c)
    {
        return c.begin();
    }

    inline unused_type const*
    begin(unused_type)
    {
        return &unused;
    }

    template <typename Container>
    inline typename result_of::iterator<Container>::type
    end(Container& c)
    {
        return c.end();
    }

    template <typename Container>
    inline typename result_of::iterator<Container const>::type
    end(Container const& c)
    {
        return c.end();
    }

    inline unused_type const*
    end(unused_type)
    {
        return &unused;
    }

    ///////////////////////////////////////////////////////////////////////////
    template <typename Iterator>
    inline typename boost::detail::iterator_traits<Iterator>::value_type
    deref(Iterator& it)
    {
        return *it;
    }

    inline unused_type
    deref(unused_type*)
    {
        return unused;
    }

    inline unused_type
    deref(unused_type const*)
    {
        return unused;
    }

    ///////////////////////////////////////////////////////////////////////////
    template <typename Iterator>
    inline Iterator
    next(Iterator& it)
    {
        return ++it;
    }

    inline unused_type
    next(unused_type*)
    {
        return &unused;
    }

    inline unused_type
    next(unused_type const*)
    {
        return &unused;
    }

    ///////////////////////////////////////////////////////////////////////////
    template <typename Iterator>
    inline bool
    compare(Iterator const& it1, Iterator const& it2)
    {
        return it1 == it2;
    }

    inline bool
    compare(unused_type*, unused_type*)
    {
        return true;
    }

}}}

#endif
