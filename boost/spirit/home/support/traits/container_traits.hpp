/*=============================================================================
    Copyright (c) 2001-2012 Joel de Guzman
    Copyright (c) 2001-2011 Hartmut Kaiser
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
#include <boost/detail/iterator.hpp>
#include <boost/mpl/has_xxx.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/range/iterator_range.hpp>

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

    template <typename T, typename Enable = void>
    struct is_container
      : mpl::bool_<
            detail::has_value_type<T>::value &&
            detail::has_iterator<T>::value &&
            detail::has_size_type<T>::value &&
            detail::has_reference<T>::value>
    {};

    template <typename T>
    struct is_container<T&> : is_container<T> {};

    template <typename T, typename Enable = void>
    struct is_iterator_range : mpl::false_ {};

    template <typename T>
    struct is_iterator_range<iterator_range<T> > : mpl::true_ {};

    ///////////////////////////////////////////////////////////////////////////
    namespace detail
    {
        template <typename T>
        struct remove_value_const
        {
            typedef T type;
        };

        template <typename T>
        struct remove_value_const<T const>
          : remove_value_const<T>
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
    template <typename Container, typename Enable = void>
    struct container_value
      : detail::remove_value_const<typename Container::value_type>
    {};

    template <typename T>
    struct container_value<T&>
      : container_value<T>
    {};

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

    ///////////////////////////////////////////////////////////////////////////
    template <typename Container, typename Enable = void>
    struct container_iterator
    {
        typedef typename Container::iterator type;
    };

    template <typename Container>
    struct container_iterator<Container&>
      : container_iterator<Container>
    {};

    template <typename Container>
    struct container_iterator<Container const>
    {
        typedef typename Container::const_iterator type;
    };

    template <typename Iterator>
    struct container_iterator<iterator_range<Iterator> >
    {
        typedef typename range_const_iterator<
              iterator_range<Iterator> >::type type;
    };

    template <>
    struct container_iterator<unused_type>
    {
        typedef unused_type const* type;
    };

    template <>
    struct container_iterator<unused_type const>
    {
        typedef unused_type const* type;
    };

    ///////////////////////////////////////////////////////////////////////////
    template <typename Container, typename T>
    bool push_back(Container& c, T const& val);

    template <typename Container, typename T, typename Enable = void>
    struct push_back_container
    {
        static bool call(Container& c, T const& val)
        {
            c.insert(c.end(), val);
            return true;
        }
    };

    template <typename Container, typename T>
    bool push_back(Container& c, T const& val)
    {
        return push_back_container<Container, T>::call(c, val);
    }

    template <typename Container>
    bool push_back(Container&, unused_type)
    {
        return true;
    }

    template <typename T>
    bool push_back(unused_type, T const&)
    {
        return true;
    }

    inline bool push_back(unused_type, unused_type)
    {
        return true;
    }

    ///////////////////////////////////////////////////////////////////////////
    template <typename Container, typename Enable = void>
    struct is_empty_container
    {
        static bool call(Container const& c)
        {
            return c.empty();
        }
    };

    template <typename Container>
    bool is_empty(Container const& c)
    {
        return is_empty_container<Container>::call(c);
    }

    inline bool is_empty(unused_type)
    {
        return true;
    }

    ///////////////////////////////////////////////////////////////////////////
    template <typename Container, typename Enable = void>
    struct begin_container
    {
        static typename container_iterator<Container>::type call(Container& c)
        {
            return c.begin();
        }
    };

    template <typename Container>
    typename container_iterator<Container>::type
    begin(Container& c)
    {
        return begin_container<Container>::call(c);
    }

    inline unused_type const*
    begin(unused_type)
    {
        return &unused;
    }

    ///////////////////////////////////////////////////////////////////////////
    template <typename Container, typename Enable = void>
    struct end_container
    {
        static typename container_iterator<Container>::type call(Container& c)
        {
            return c.end();
        }
    };

    template <typename Container>
    inline typename container_iterator<Container>::type
    end(Container& c)
    {
        return end_container<Container>::call(c);
    }

    inline unused_type const*
    end(unused_type)
    {
        return &unused;
    }

    ///////////////////////////////////////////////////////////////////////////
    template <typename Iterator, typename Enable = void>
    struct deref_iterator
    {
        typedef typename boost::detail::iterator_traits<Iterator>::reference type;
        static type call(Iterator& it)
        {
            return *it;
        }
    };

    template <typename Iterator>
    typename deref_iterator<Iterator>::type
    deref(Iterator& it)
    {
        return deref_iterator<Iterator>::call(it);
    }

    inline unused_type
    deref(unused_type const*)
    {
        return unused;
    }

    ///////////////////////////////////////////////////////////////////////////
    template <typename Iterator, typename Enable = void>
    struct next_iterator
    {
        static void call(Iterator& it)
        {
            ++it;
        }
    };

    template <typename Iterator>
    void next(Iterator& it)
    {
        next_iterator<Iterator>::call(it);
    }

    inline void next(unused_type const*)
    {
        // do nothing
    }

    ///////////////////////////////////////////////////////////////////////////
    template <typename Iterator, typename Enable = void>
    struct compare_iterators
    {
        static bool call(Iterator const& it1, Iterator const& it2)
        {
            return it1 == it2;
        }
    };

    template <typename Iterator>
    bool compare(Iterator& it1, Iterator& it2)
    {
        return compare_iterators<Iterator>::call(it1, it2);
    }

    inline bool compare(unused_type const*, unused_type const*)
    {
        return false;
    }

    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    struct build_container : mpl::identity<std::vector<T>> {};

    template <>
    struct build_container<unused_type> : mpl::identity<unused_type> {};
}}}

#endif
