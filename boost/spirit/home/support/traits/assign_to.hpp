/*=============================================================================
    Copyright (c) 2001-2013 Joel de Guzman
    http://spirit.sourceforge.net/

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_ASSIGN_TO_APR_16_2006_0812PM)
#define BOOST_SPIRIT_ASSIGN_TO_APR_16_2006_0812PM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/support/traits/attribute_category.hpp>
#include <boost/fusion/include/is_sequence.hpp>
#include <boost/fusion/include/deque.hpp>
#include <boost/fusion/include/front.hpp>
#include <boost/fusion/include/size.hpp>

namespace boost { namespace spirit { namespace traits
{
    template <typename T, typename Attribute>
    void assign_to(T const& val, Attribute& attr);

    template <typename Attribute>
    inline void assign_to(unused_type, Attribute& attr) {}

    template <typename T>
    inline void assign_to(T const& val, unused_type) {}

    inline void assign_to(unused_type, unused_type) {}

    template <typename Iterator, typename Attribute>
    void
    assign_to(Iterator const& first, Iterator const& last, Attribute& attr);

    template <typename Iterator>
    inline void
    assign_to(Iterator const& first, Iterator const& last, unused_type) {}

    namespace detail
    {
        template <typename T, typename Attribute>
        inline void
        assign_to(T const& val, Attribute& attr, unused_attribute)
        {
        }

        template <typename T, typename Attribute>
        inline void
        assign_to(T const& val, Attribute& attr, plain_attribute)
        {
            attr = val;
        }

        template <typename T, typename Attribute>
        inline typename enable_if<is_container<T>>::type
        assign_to(T const& val, Attribute& attr, container_attribute)
        {
            attr.assign(val.begin(), val.end());
        }

        namespace detail
        {
            template <typename A, typename B>
            struct has_same_size
              : mpl::bool_<(
                    fusion::result_of::size<A>::value ==
                    fusion::result_of::size<B>::value
                )>
            {};

            template <typename T, std::size_t N>
            struct has_size
              : mpl::bool_<(fusion::result_of::size<T>::value == N)>
            {};

            template <typename A, typename B>
            struct is_same_size_sequence
              : mpl::and_<
                    fusion::traits::is_sequence<A>
                  , fusion::traits::is_sequence<B>
                  , has_same_size<A, B>
                >
            {};

            template <typename Seq>
            struct is_size_one_sequence
              : mpl::and_<
                    fusion::traits::is_sequence<Seq>
                  , has_size<Seq, 1>
                >
            {};
        }

        template <typename T, typename Attribute>
        inline typename enable_if<
            detail::is_same_size_sequence<Attribute, T>
        >::type
        assign_to(T const& val, Attribute& attr, tuple_attribute)
        {
            fusion::copy(val, attr);
        }

        template <typename T, typename Attribute>
        inline typename enable_if<
            detail::is_size_one_sequence<Attribute>
        >::type
        assign_to(T const& val, Attribute& attr, tuple_attribute)
        {
            traits::assign_to(val, fusion::front(attr));
        }

        template <typename T, typename Attribute>
        inline void
        assign_to(T const& val, Attribute& attr, variant_attribute, mpl::false_)
        {
            attr = val;
        }

        template <typename T, typename Attribute>
        inline void
        assign_to(T const& val, Attribute& attr, variant_attribute, mpl::true_)
        {
            attr = fusion::front(val);
        }

        template <typename T, typename Attribute>
        inline void
        assign_to(T const& val, Attribute& attr, variant_attribute tag)
        {
            assign_to(val, attr, tag, detail::is_size_one_sequence<T>());
        }

        template <typename Iterator>
        inline void
        assign_to(Iterator const&, Iterator const&, unused_type, unused_attribute)
        {
        }

        template <typename Iterator, typename Attribute>
        inline void
        assign_to(Iterator const& first, Iterator const& last, Attribute& attr, container_attribute)
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
    assign_to(T const& val, Attribute& attr)
    {
        detail::assign_to(val, attr, typename attribute_category<Attribute>::type());
    }

    template <typename Iterator, typename Attribute>
    inline void
    assign_to(Iterator const& first, Iterator const& last, Attribute& attr)
    {
        detail::assign_to(first, last, attr, typename attribute_category<Attribute>::type());
    }
}}}

#endif
