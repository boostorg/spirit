/*=============================================================================
    Copyright (c) 2001-2013 Joel de Guzman
    Copyright (c) 2013 Agustín Bergé
    http://spirit.sourceforge.net/

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_MOVE_TO_JAN_17_2013_0859PM)
#define BOOST_SPIRIT_MOVE_TO_JAN_17_2013_0859PM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/support/traits/attribute_category.hpp>
#include <boost/spirit/home/support/traits/tuple_traits.hpp>
#include <boost/fusion/include/is_sequence.hpp>
#include <boost/fusion/include/front.hpp>
#include <boost/fusion/include/size.hpp>
#include <boost/fusion/include/move.hpp>
#include <utility>

namespace boost { namespace spirit { namespace traits
{
    template <typename Source, typename Dest>
    void move_to(Source&& src, Dest& dest);

    template <typename Dest>
    inline void move_to(unused_type, Dest&) {}

    template <typename Source>
    inline void move_to(Source&, unused_type) {}

    inline void move_to(unused_type, unused_type) {}

    template <typename Iterator, typename Dest>
    void
    move_to(Iterator first, Iterator last, Dest& dest);

    template <typename Iterator>
    inline void
    move_to(Iterator, Iterator, unused_type) {}

    namespace detail
    {
        template <typename Source, typename Dest>
        inline void
        move_to(Source&&, Dest&, unused_attribute) {}

        template <typename Source, typename Dest>
        inline void
        move_to(Source&& src, Dest& dest, plain_attribute)
        {
            dest = std::move(src);
        }

        template <typename Source, typename Dest>
        inline typename enable_if<is_container<Source>>::type
        move_to(Source&& src, Dest& dest, container_attribute)
        {
            traits::move_to(src.begin(), src.end(), dest);
        }

        template <typename Source, typename Dest>
        inline typename enable_if<
            is_same_size_sequence<Dest, Source>
        >::type
        move_to(Source&& src, Dest& dest, tuple_attribute)
        {
            fusion::move(std::move(src), dest);
        }

        template <typename Source, typename Dest>
        inline typename enable_if<
            is_size_one_sequence<Dest>
        >::type
        move_to(Source&& src, Dest& dest, tuple_attribute)
        {
            traits::move_to(src, fusion::front(dest));
        }

        template <typename Source, typename Dest>
        inline void
        move_to(Source&& src, Dest& dest, variant_attribute, mpl::false_)
        {
            dest = std::move(src);
        }

        template <typename Source, typename Dest>
        inline void
        move_to(Source&& src, Dest& dest, variant_attribute, mpl::true_)
        {
            dest = std::move(fusion::front(src));
        }

        template <typename Source, typename Dest>
        inline void
        move_to(Source&& src, Dest& dest, variant_attribute tag)
        {
            move_to(src, dest, tag, is_size_one_sequence<Source>());
        }

        template <typename Iterator>
        inline void
        move_to(Iterator, Iterator, unused_type, unused_attribute) {}

        template <typename Iterator, typename Dest>
        inline void
        move_to(Iterator first, Iterator last, Dest& dest, container_attribute)
        {
            if (is_empty(dest))
                dest = Dest(first, last);
            else
                append(dest, first, last);
        }
    }

    template <typename Source, typename Dest>
    inline void
    move_to(Source&& src, Dest& dest)
    {
        detail::move_to(std::move(src), dest
          , typename attribute_category<Dest>::type());
    }

    template <typename T>
    inline void move_to(T& src, T& dest)
    {
        dest = std::move(src);
    }

    template <typename T>
    inline void move_to(T const& src, T& dest)
    {
        dest = std::move(src);
    }

    template <typename T>
    inline void move_to(T&& src, T& dest)
    {
        dest = std::move(src);
    }

    template <typename Iterator, typename Dest>
    inline void
    move_to(Iterator first, Iterator last, Dest& dest)
    {
        // $$$ Use std::move_iterator when iterator is not a const-iterator $$$
        detail::move_to(first, last, dest, typename attribute_category<Dest>::type());
    }
}}}

#endif
