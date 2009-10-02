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
#include <boost/variant.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/repeat.hpp>

namespace boost { namespace spirit { namespace traits
{
    ///////////////////////////////////////////////////////////////////////////
    //  This file contains some container utils for stl containers. The
    //  utilities provided also accept spirit's unused_type; all no-ops.
    //  Compiler optimization will easily strip these away.
    ///////////////////////////////////////////////////////////////////////////

    namespace detail
    {
        BOOST_MPL_HAS_XXX_TRAIT_DEF(value_type)
        BOOST_MPL_HAS_XXX_TRAIT_DEF(iterator)
        BOOST_MPL_HAS_XXX_TRAIT_DEF(size_type)
        BOOST_MPL_HAS_XXX_TRAIT_DEF(reference)
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

   template <typename T>
   struct is_container<optional<T> > 
     : is_container<T> {};

#define BOOST_SPIRIT_IS_CONTAINER(z, N, data)                                 \
       is_container<BOOST_PP_CAT(T, N)>::value ||                             \
   /***/

   template <BOOST_VARIANT_ENUM_PARAMS(typename T)>
   struct is_container<variant<BOOST_VARIANT_ENUM_PARAMS(T)> > 
     : mpl::bool_<BOOST_PP_REPEAT(BOOST_VARIANT_LIMIT_TYPES
         , BOOST_SPIRIT_IS_CONTAINER, _) false> {};

#undef BOOST_SPIRIT_IS_CONTAINER

    ///////////////////////////////////////////////////////////////////////////
    namespace detail
    {
        template <typename T>
        struct remove_value_const
        {
            typedef T type;
        };

        template <typename T>
        struct remove_value_const<T const> : remove_value_const<T>
        {};

        template <typename F, typename S>
        struct remove_value_const<std::pair<F, S> >
        {
            typedef typename remove_value_const<F>::type first_type;
            typedef typename remove_value_const<S>::type second_type;
            typedef std::pair<first_type, second_type> type;
        };
    }

    ///////////////////////////////////////////////////////////////////////
    template <typename Container>
    struct container_value
    {
        typedef typename detail::remove_value_const<
            typename Container::value_type>::type
        type;
    };

    // this will be instantiated if the optional holds a container
    template <typename T>
    struct container_value<optional<T> > : container_value<T> {};

    // this will be instantiated if the variant holds a container
    template <BOOST_VARIANT_ENUM_PARAMS(typename T)>
    struct container_value<variant<BOOST_VARIANT_ENUM_PARAMS(T)> >
    {
        typedef typename 
            variant<BOOST_VARIANT_ENUM_PARAMS(T)>::types 
        types;
        typedef typename 
            mpl::find_if<types, is_container<mpl::_1> >::type 
        iter;

        typedef typename container_value<
            typename mpl::if_<
                is_same<iter, typename mpl::end<types>::type>
              , unused_type, typename mpl::deref<iter>::type
            >::type
        >::type type;
    };

    template <>
    struct container_value<unused_type>
    {
        typedef unused_type type;
    };

    template <>
    struct container_value<unused_type const>
    {
        typedef unused_type type;
    };

    ///////////////////////////////////////////////////////////////////////
        
    namespace result_of
    {
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

        ///////////////////////////////////////////////////////////////////////
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
    template <typename T>
    T const& optional_value(T const& v)
    {
        return v;
    }

    template <typename T>
    T const& optional_value(boost::optional<T> const& v)
    {
        return boost::get<T>(v);
    }

    inline unused_type optional_value(unused_type)
    {
        return unused;
    }

    template <typename T>
    bool has_optional_value(T const&)
    {
        return true;
    }

    template <typename T>
    bool has_optional_value(boost::optional<T> const& v)
    {
        return v;
    }

    inline bool has_optional_value(unused_type)
    {
        return true;
    }

    ///////////////////////////////////////////////////////////////////////////
    template <typename Container, typename T>
    inline void push_back(Container& c, T const& val)
    {
        c.insert(c.end(), val);
    }

    template <typename Container, typename T>
    inline void push_back(optional<Container>& c, T const& val)
    {
        if (!c)
            c = Container();
        push_back(boost::get<Container>(c), val);
    }

    namespace detail
    {
        template <typename T>
        struct push_back_visitor : public static_visitor<>
        {
            push_back_visitor(T const& t) : t_(t) {}

            template <typename Container>
            void push_back_impl(Container& c, mpl::true_) const
            {
                push_back(c, t_);
            }

            template <typename T_>
            void push_back_impl(T_&, mpl::false_) const
            {
                // this variant doesn't hold a container
                BOOST_ASSERT(false);
            }

            template <typename T_>
            void operator()(T_& c) const
            {
                push_back_impl(c, typename is_container<T_>::type());
            }

            T const& t_;
        };
    }

    template <BOOST_VARIANT_ENUM_PARAMS(typename T_), typename T>
    inline void push_back(variant<BOOST_VARIANT_ENUM_PARAMS(T_)>& c
      , T const& val)
    {
        apply_visitor(detail::push_back_visitor<T>(val), c);
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
    inline typename boost::detail::iterator_traits<Iterator>::reference
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

    inline bool
    compare(unused_type const*, unused_type const*)
    {
        return true;
    }

}}}

#endif
