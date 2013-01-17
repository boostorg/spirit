/*=============================================================================
    Copyright (c) 2001-2013 Joel de Guzman
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
#include <boost/spirit/home/support/traits/sequence_traits.hpp>
#include <boost/fusion/include/is_sequence.hpp>
#include <boost/fusion/include/front.hpp>
#include <boost/fusion/include/size.hpp>
#include <utility>

namespace boost { namespace spirit { namespace traits
{
    template <typename T, typename Attribute>
    void move_to(T&& val, Attribute& attr);

    template <typename Attribute>
    inline void move_to(unused_type, Attribute& attr) {}

    template <typename T>
    inline void move_to(T const& val, unused_type) {}

    inline void move_to(unused_type, unused_type) {}

    template <typename Iterator, typename Attribute>
    void
    move_to(Iterator const& first, Iterator const& last, Attribute& attr);

    template <typename Iterator>
    inline void
    move_to(Iterator const& first, Iterator const& last, unused_type) {}

    namespace detail
    {
        template <typename T, typename Attribute>
        inline void
        move_to(T const& val, Attribute& attr, unused_attribute)
        {
        }

        template <typename T, typename Attribute>
        inline void
        move_to(T&& val, Attribute& attr, plain_attribute)
        {
            attr = std::move(val);
        }

        template <typename T, typename Attribute>
        inline typename enable_if<is_container<T>>::type
        move_to(T const& val, Attribute& attr, container_attribute)
        {
            attr.assign(val.begin(), val.end());
        }

        template <typename T>
        inline typename enable_if<is_container<T>>::type
        move_to(T&& val, T& attr, container_attribute)
        {
            attr = std::move(val);
        }

        template <typename T, typename Attribute>
        inline typename enable_if<
            is_same_size_sequence<Attribute, T>
        >::type
        move_to(T const& val, Attribute& attr, tuple_attribute)
        {
            // $$$ TODO There should be a fusion::move $$$
            fusion::copy(val, attr);
        }

        template <typename T, typename Attribute>
        inline typename enable_if<
            is_size_one_sequence<Attribute>
        >::type
        move_to(T const& val, Attribute& attr, tuple_attribute)
        {
            traits::move_to(val, fusion::front(attr));
        }

        template <typename T>
        inline void
        move_to(T&& val, T& attr, tuple_attribute)
        {
            attr = std::move(val);
        }

        template <typename T, typename Attribute>
        inline void
        move_to(T const& val, Attribute& attr, variant_attribute, mpl::false_)
        {
            attr = val;
        }

        template <typename T, typename Attribute>
        inline void
        move_to(T const& val, Attribute& attr, variant_attribute, mpl::true_)
        {
            attr = fusion::front(val);
        }

        template <typename T, typename Attribute>
        inline void
        move_to(T const& val, Attribute& attr, variant_attribute tag)
        {
            move_to(val, attr, tag, is_size_one_sequence<T>());
        }

        template <typename T>
        inline void
        move_to(T&& val, T& attr, variant_attribute tag)
        {
            attr = std::move(val);
        }

        template <typename Iterator>
        inline void
        move_to(Iterator const&, Iterator const&, unused_type, unused_attribute)
        {
        }

        template <typename Iterator, typename Attribute>
        inline void
        move_to(Iterator const& first, Iterator const& last, Attribute& attr, container_attribute)
        {
            // $$$ fixme: use CP traits! $$$
            if (attr.empty())
            {
                attr = Attribute(first, last);
            }
            else
            {
                attr.reserve(attr.size() + std::distance(first, last));
                for (Iterator i = first; i != last; ++i)
                    attr.push_back(*i);
            }
        }
    }

    template <typename T, typename Attribute>
    inline void
    move_to(T&& val, Attribute& attr)
    {
        detail::move_to(val, attr, typename attribute_category<Attribute>::type());
    }

    template <typename Iterator, typename Attribute>
    inline void
    move_to(Iterator const& first, Iterator const& last, Attribute& attr)
    {
        detail::move_to(first, last, attr, typename attribute_category<Attribute>::type());
    }

}}}

#endif
